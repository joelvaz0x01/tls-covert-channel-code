/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fountain_code/decoder.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <rand64/rand64.h>
#include <rand64/system.h>

/**
 * Performs a complete encode-decode-verify cycle for a given number of source blocks.
 *
 * @param n Number of source blocks to test.
 */
void run_test(uint64_t n) {
  seed64_system();
  printf("Testing with n = %4" PRIu64 " blocks...  ", n);

  if (n < 2 || n > MAX_BLOCKS) {
    fprintf(stderr, "Error: invalid n = %" PRIu64 " (must be between 2 and %" PRIu64 ")\n", n, (uint64_t)MAX_BLOCKS);
    assert(0);
  }

  block_t* src = malloc((size_t)n * sizeof(block_t));
  assert(src != NULL);

  /* fill blocks with some pseudo-random data */
  unsigned char* src_bytes = (unsigned char*)src;
  for (size_t i = 0; i < (size_t)n * sizeof(block_t); i++) {
    src_bytes[i] = (unsigned char)(rand64_between(0, 255));
  }

  int m = (int)round(2.5 * log((double)n) + EULER);
  if (m < 1) m = 1;
  if (m >= n) m = n - 1;

  /* decode blocks on-the-fly */
  decoder_t* dec = calloc(1, sizeof(decoder_t));
  assert(NULL != dec);
  decoder_init(dec, n);

  uint64_t n_words = (n + 63) / 64;
  int total_sent = 0;

  /* keep sending packets until the decoder has found enough pivots to solve the system */
  while (dec->remaining != 0) {
    seed64_system();
    packet_t pkt = encode_packet(total_sent, src, n, m, n_words);
    decoder_feed(dec, &pkt);
    total_sent++;

    /* safety break to prevent infinite loop if implementation is broken */
    if (total_sent > 10000) {
      fprintf(stderr, "Error: Decoder failed to converge after 10,000 packets\n");
      assert(0);
    }
  }

  /* back-substitution to recover blocks */
  block_t* out = malloc((size_t)n * sizeof(block_t));
  assert(NULL != out);
  decoder_solve(dec, out);

  /* verify result */
  int match = (0 == memcmp(src, out, (size_t)n * sizeof(block_t)));

  if (!match) {
    fprintf(stderr, "FAILURE: Reconstructed data does not match original for n=%" PRIu64 "\n", n);
    assert(match);
  }

  printf("SUCCESS (%5d packets generated)\n", total_sent);

  free(src);
  free(out);
  free(dec);
}

int main(void) {
  printf("\nStarting Fountain Code Implementation Tests...\n");
  printf("---------------------------------------------------------------------\n");

  /* test a variety of sizes */
  uint64_t n;
  for (n = 2; n <= MAX_BLOCKS; n *= 2) {
    run_test(n);
  }

  if (n / 2 != MAX_BLOCKS)
    run_test(MAX_BLOCKS);

  printf("---------------------------------------------------------------------\n");
  printf("All tests passed successfully!\n\n");

  return 0;
}
