/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Toy example that demonstrates the Fountain Code encoder and decoder.
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <fountain_code/decode.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include <rand/rand128.h>
#include <rand/system.h>

#include <utils/decoder.h>
#include <utils/encoder.h>
#include <utils/file.h>
#include <utils/print.h>

#define INPUT_FILE  "input.txt"
#define OUTPUT_FILE "reconstructed.txt"

static bool success = true;

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
  /* ensure 'input.txt' exists and has content. */
  {
    FILE* chk = fopen(INPUT_FILE, "rb");
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
      FILE* wfp = fopen(INPUT_FILE, "w");
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

  uint64_t n = calculate_n(INPUT_FILE, true);
  if (n == 0 || n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    goto cleanup;
  }

  /* print banner */
  printf("======================================================================\n");
  printf(" Fountain Code File-Transfer Demo\n");
  printf("======================================================================\n");
  printf(" Input file    : %s\n", INPUT_FILE);
  printf(" Source blocks : n = %" PRIu64 " blocks (each with %d bits)\n", n, FC_BLOCK_SIZE);
  printf(" Degree        : m = round(%.1f * ln(%" PRIu64 ") + Euler-Mascheroni)\n", ALPHA, n);
  printf("======================================================================\n\n");

  uint64_t m = generate_m(n);
  uint64_t n_words = init_program(n, m);

  /* print source blocks */
  printf("[ Source blocks ]\n");
  for (uint64_t i = 0; i < n; i++) {
    if (-1 == read_file_part(INPUT_FILE, i, &buffer[0])) {
      fprintf(stderr, "\nError: failed to read block %" PRIu64 " from input file\n", i);
      goto cleanup;
    }
    printf("  [%2" PRIu64 "]  hex: ", i);
    print_hex_bits(&buffer[0], FC_BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii_bits(&buffer[0], FC_BLOCK_SIZE);
    printf("\"\n");
  }
  putchar('\n');

  seed128_system();
  int total_sent = 0;
  int total_useful = 0;

  /* column header */
  printf("[ Fountain packets ]\n");
  printf("  %-4s  %-*s  %-*s  %s\n", "Pkt#", (int)n, "Selector", (FC_BLOCK_SIZE >> 2), "Encoded data (hex)", "Status");

  /* separator line */
  printf("  ");
  for (int k = 0; k < 6 + (int)n + 2 + (FC_BLOCK_SIZE >> 2) + 2 + 30; k++) putchar('-');
  putchar('\n');

  while (0 != dec->remaining) {
    uint64_t k = construct_k(rand128(), m, n, n_words, INPUT_FILE);
    if (0 == k) {
      fprintf(stderr, "[-] construct_k failed\n");
      goto cleanup;
    }
    encode_packet(g_scratch_pkt, total_sent, k, buffer);

    printf("  #%-3d  ", g_scratch_pkt->id);
    print_sel(dec->scratch_sel, n);
    printf("  ");
    print_hex_bits(&g_scratch_pkt->data, FC_BLOCK_SIZE);

    bool useful = decoder_feed(dec, g_scratch_pkt);
    if (useful) total_useful++;
    total_sent++;

    printf("  %s  (remaining=%" PRIu64 ")\n", useful ? "[ new pivot ]" : "[ redundant ]", dec->remaining);
  }

  printf(
    "\n  Packets sent: %d  |  useful (new pivot): %d  |  redundant: %d\n\n",
    total_sent,
    total_useful,
    total_sent - total_useful
  );

  /* back-substitution to recover individual source blocks (in-place) */
  decoder_solve(dec, NULL);

  /* compare reconstructed blocks against originals and write output */
  printf("[ Reconstructed blocks ]\n");
  for (uint64_t i = 0; i < n; i++) {
    if (-1 == read_file_part(INPUT_FILE, i, &buffer[0])) {
      fprintf(stderr, "\nError: failed to read block %" PRIu64 " from input file\n", i);
      break;
    }
    bool ok = (0 == memcmp(&dec->pivot_data[i], &buffer[0], sizeof(block_t)));
    if (!ok) success = false;

    printf("  [%2" PRIu64 "]  hex: ", i);
    print_hex_bits(&dec->pivot_data[i], FC_BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii_bits(&dec->pivot_data[i], FC_BLOCK_SIZE);
    printf("\"  [%s]\n", ok ? " OK " : "FAIL");

    size_t to_write = bytes_to_write(i, n);
    if (-1 == save_decoder(OUTPUT_FILE, dec->pivot_data[i], to_write)) {
      fprintf(stderr, "\nError: failed to write block %" PRIu64 " to output file\n", i);
      success = false;
      break;
    }
  }

  close_files();

  printf(
    "\n  Reconstruction : %s\n",
    success ? "SUCCESS — original file recovered perfectly!" : "FAILURE — one or more blocks do not match!"
  );

  if (!success)
    remove(OUTPUT_FILE);
  else
    printf("  Output written : %s\n\n", OUTPUT_FILE);

cleanup:
  close_files();
  finalize_program();

  return success ? 0 : 1;
}
