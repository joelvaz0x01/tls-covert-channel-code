/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Toy example that demonstrates the Fountain Code encoder and decoder.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fountain_code/decoder.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <rand/rand64.h>
#include <rand/system.h>
#include <utils/utils.h>

/**
 * Prints the given selector as a binary string of exactly n characters.
 *
 * @param v Pointer to the selector to print.
 * @param n Number of characters to print.
 */
static inline void print_sel(const vec_t* v, uint64_t n) {
  for (uint64_t i = 0; i < n; i++) putchar(vec_test(v, i) ? '1' : '0');
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
  uint64_t n = (uint64_t)((file_bits + FC_BLOCK_SIZE - 1) / FC_BLOCK_SIZE);
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
  for (uint64_t i = 0; i < n; i++) {
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
  uint64_t m = generate_m(n);

  /* print banner */
  printf("======================================================================\n");
  printf(" Fountain Code File-Transfer Demo\n");
  printf("======================================================================\n");
  printf(" Input file    : %s (%ld bits)\n", input_file, file_bits);
  printf(" Source blocks : n = %" PRIu64 " blocks (each with %d bits)\n", n, FC_BLOCK_SIZE);
  printf(" Degree        : m = round(%.1f * ln(%" PRIu64 ") + Euler-Mascheroni)\n", ALPHA, n);
  printf("======================================================================\n\n");

  /* print source blocks */
  printf("[ Source blocks ]\n");
  for (uint64_t i = 0; i < n; i++) {
    printf("  [%2" PRIu64 "]  hex: ", i);
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

  uint64_t n_words = (n + 63) / 64;
  int total_sent = 0;
  int total_useful = 0;

  /* column header */
  printf("[ Fountain packets ]\n");
  printf("  %-4s  %-*s  %-*s  %s\n", "Pkt#", (int)n, "Selector", (FC_BLOCK_SIZE >> 2), "Encoded data (hex)", "Status");

  /* separator line */
  printf("  ");
  for (int k = 0; k < 6 + (int)n + 2 + (FC_BLOCK_SIZE >> 2) + 2 + 30; k++) putchar('-');
  putchar('\n');

  while (dec->remaining != 0) {
    uint64_t seed = rand64();
    packet_t pkt = encode_packet(total_sent, seed, src, n, m, n_words);
    total_sent++;

    bool useful = decoder_feed(dec, &pkt);
    if (useful) total_useful++;

    printf("  #%-3d  ", pkt.id);
    print_sel(&pkt.selector, n);
    printf("  ");
    print_hex_bits(&pkt.data, FC_BLOCK_SIZE);
    printf("  %s  (remaining=%" PRIu64 ")\n", useful ? "[ new pivot ]" : "[ redundant ]", dec->remaining);
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
  bool all_ok = true;
  for (uint64_t i = 0; i < n; i++) {
    bool ok = (memcmp(&out[i], &src[i], sizeof(block_t)) == 0);
    if (!ok) all_ok = false;
    printf("  [%2" PRIu64 "]  hex: ", i);
    print_hex_bits(&out[i], FC_BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii_bits(&out[i], FC_BLOCK_SIZE);
    printf("\"  [%s]\n", ok ? " OK " : "FAIL");
  }

  /* write output file */
  fp = fopen(output_file, "wb");
  if (fp) {
    long remaining_bits = original_file_bits;
    for (uint64_t i = 0; i < n; i++) {
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
