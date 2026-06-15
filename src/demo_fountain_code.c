/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Toy example that demonstrates the Fountain Code encoder and decoder.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fountain_code/decoder.h>
#include <fountain_code/encoder.h>
#include <fountain_code/utils.h>
#include <rand64/system.h>
#include <utils/utils.h>

#include "rand64/rand64.h"

/**
 * Prints the given selector as a binary string of exactly n characters.
 *
 * @param v Pointer to the selector to print.
 * @param n Number of characters to print.
 */
static inline void print_sel(const vec_t* v, int n) {
  for (int i = 0; i < n; i++) putchar(vec_test(v, i) ? '1' : '0');
}

int main(void) {
  const char* input_file = "input.txt";
  const char* output_file = "reconstructed.txt";

  /* ensure 'input.txt' exists and has content. */
  {
    FILE* chk = fopen(input_file, "rb");
    int needs_content;
    if (NULL == chk) {
      needs_content = 1; /* file does not exist */
    } else {
      fseek(chk, 0, SEEK_END);
      needs_content = (0 == ftell(chk)); /* file exists but is empty */
      fclose(chk);
    }

    /* put default text into empty file */
    if (needs_content) {
      FILE* wfp = fopen(input_file, "w");
      if (!wfp) {
        perror("fopen input.txt");
        return 1;
      }
      fputs(
        "This is a live fountain code file-transfer demonstration made for "
        "'Low-Bandwidth Covert Channels Using The Initial TLS Handshake Messages' dissertation.\n"
        "2026 Joel Vaz @ Universidade de Aveiro\n",
        wfp
      );
      fclose(wfp);
    }
  }

  /* read input file into zero-padded source blocks */
  FILE* fp = fopen(input_file, "rb");
  if (!fp) {
    perror("fopen");
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  long file_bits = ftell(fp) << 3;
  long original_file_bits = file_bits;
  rewind(fp);

  /* Calculate number of blocks based on bit size */
  int n = (int)((file_bits + FC_BLOCK_SIZE - 1) / FC_BLOCK_SIZE);
  if (n < 2 || n > MAX_BLOCKS) {
    fprintf(stderr, "File must produce between 2 and %d blocks.\n", MAX_BLOCKS);
    fclose(fp);
    return 1;
  }

  block_t* src = calloc((size_t)n, sizeof(block_t));
  if (!src) {
    perror("calloc");
    fclose(fp);
    return 1;
  }

  /* Reading file into blocks. The data is stored in the uint64_t words of block_t. */
  for (int i = 0; i < n; i++) {
    int to_read = FC_BLOCK_SIZE;
    if (file_bits < to_read) to_read = (int)file_bits;
    if (to_read > 0) {
      if (fread(&src[i], 1, (size_t)(to_read >> 3), fp) != (size_t)(to_read >> 3)) {
        perror("fread");
        free(src);
        fclose(fp);
        return 1;
      }
      file_bits -= to_read;
    }
  }
  fclose(fp);
  file_bits = original_file_bits;

  /* compute packet degree m */
  double alpha = 2.5;
  int m = (int)round(alpha * log((double)n) + EULER);
  if (m < 1) m = 1;
  if (m >= n) m = n - 1;

  /* print banner */
  printf("======================================================================\n");
  printf(" Fountain Code File-Transfer Demo\n");
  printf("======================================================================\n");
  printf(" Input file    : %s (%ld bits)\n", input_file, file_bits);
  printf(" Source blocks : n = %d blocks (each with %d bits)\n", n, FC_BLOCK_SIZE);
  printf(" Degree        : m = round(%.1f * ln(%d) + Euler-Mascheroni)\n", alpha, n);
  printf("======================================================================\n\n");

  /* print source blocks */
  printf("[ Source blocks ]\n");
  for (int i = 0; i < n; i++) {
    printf("  [%2d]  hex: ", i);
    print_hex_bits(&src[i], FC_BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii_bits(&src[i], FC_BLOCK_SIZE);
    printf("\"\n");
  }
  putchar('\n');

  /* encode packets and decode on-the-fly */
  seed64_system();

  decoder_t* dec = calloc(1, sizeof(decoder_t));
  if (!dec) {
    perror("calloc decoder");
    free(src);
    return 1;
  }
  decoder_init(dec, n);

  int n_words = (n + 63) / 64;
  int total_sent = 0;
  int total_useful = 0;

  /* column header */
  printf("[ Fountain packets ]\n");
  printf("  %-4s  %-*s  %-*s  %s\n", "Pkt#", n, "Selector", (FC_BLOCK_SIZE >> 2), "Encoded data (hex)", "Status");

  /* separator line */
  printf("  ");
  for (int k = 0; k < 6 + n + 2 + (FC_BLOCK_SIZE >> 2) + 2 + 30; k++) putchar('-');
  putchar('\n');

  while (dec->remaining > 0) {
    packet_t pkt = encode_packet(total_sent, src, n, m, n_words);
    total_sent++;

    int useful = decoder_feed(dec, &pkt);
    if (useful) total_useful++;

    printf("  #%-3d  ", pkt.id);
    print_sel(&pkt.selector, n);
    printf("  ");
    print_hex_bits(&pkt.data, FC_BLOCK_SIZE);
    printf("  %s  (remaining=%d)\n", useful ? "[ new pivot ]" : "[ redundant ]", dec->remaining);
  }

  printf(
    "\n  Packets sent: %d  |  useful (new pivot): %d  |  redundant: %d\n\n",
    total_sent,
    total_useful,
    total_sent - total_useful
  );

  /* back-substitution to recover individual source blocks */
  block_t* out = calloc((size_t)n, sizeof(block_t));
  if (!out) {
    perror("calloc out");
    free(dec);
    free(src);
    return 1;
  }

  decoder_solve(dec, out);

  /* print reconstructed blocks */
  printf("[ Reconstructed blocks ]\n");
  int all_ok = 1;
  for (int i = 0; i < n; i++) {
    int ok = (memcmp(&out[i], &src[i], sizeof(block_t)) == 0);
    if (!ok) all_ok = 0;
    printf("  [%2d]  hex: ", i);
    print_hex_bits(&out[i], FC_BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii_bits(&out[i], FC_BLOCK_SIZE);
    printf("\"  [%s]\n", ok ? " OK " : "FAIL");
  }

  /* write output file */
  fp = fopen(output_file, "wb");
  if (fp) {
    long remaining_bits = original_file_bits;
    for (int i = 0; i < n; i++) {
      int to_write = FC_BLOCK_SIZE;
      if (remaining_bits < to_write) to_write = (int)remaining_bits;
      if (to_write > 0) {
        fwrite(&out[i], 1, (size_t)(to_write >> 3), fp);
        remaining_bits -= to_write;
      }
    }
    fclose(fp);
  }

  printf(
    "\n  Reconstruction : %s\n",
    all_ok ? "SUCCESS — original file recovered perfectly!" : "FAILURE — one or more blocks do not match!"
  );
  printf("  Output written : %s\n\n", output_file);

  free(out);
  free(dec);
  free(src);

  return all_ok ? 0 : 1;
}
