/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fountain_code/decode.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/vec_ops.h>
#include <rand/rand128.h>
#include <rand/system.h>
#include <utils/utils.h>

void run_test(uint64_t n) {
  printf("Testing with n = %4" PRIu64 " blocks...  ", n);

  if (n < 2 || n > MAX_BLOCKS) {
    fprintf(stderr, "Error: invalid n = %" PRIu64 " (must be between 2 and %" PRIu64 ")\n", n, (uint64_t)MAX_BLOCKS);
    assert(0);
  }

  uint64_t m = generate_m(n);
  uint64_t n_words = init_program(n, m);

  block_t* src = malloc((size_t)n * sizeof(block_t));
  assert(src != NULL);

  unsigned char* src_bytes = (unsigned char*)src;
  for (size_t i = 0; i < (size_t)n * sizeof(block_t); i++) {
    src_bytes[i] = (unsigned char)(rand128_between(0, 255));
  }

  seed64_system();
  int total_sent = 0;

  /* encode */
  while (dec->remaining != 0) {
    uint64_t k = generate_k(m, rand128(), n, k_list);
    packet_t pkt;
    vec_zero(dec->scratch_sel, n_words);
    for (uint64_t i = 0; i < k; i++) {
      vec_set(dec->scratch_sel, k_list[i]);
      buffer[i] = src[k_list[i]];  // review
    }
    encode_packet(&pkt, total_sent, k, buffer);
    decoder_feed(dec, &pkt);
    total_sent++;

    if (total_sent > 10000) {
      fprintf(stderr, "Error: Decoder failed to converge after 10,000 packets\n");
      assert(0);
    }
  }

  block_t* out = malloc((size_t)n * sizeof(block_t));
  assert(NULL != out);

  /* decode */
  decoder_solve(dec, out);

  bool match = (0 == memcmp(src, out, (size_t)n * sizeof(block_t)));
  if (!match) {
    fprintf(stderr, "FAILURE: Reconstructed data does not match original for n=%" PRIu64 "\n", n);
    assert(match);
  }

  printf("SUCCESS (%5d packets generated)\n", total_sent);
  finalize_program();

  free(src);
  free(out);

  src = NULL;
  out = NULL;
}

int main(void) {
  printf("\nStarting Fountain Code Implementation Tests...\n");
  printf("---------------------------------------------------------------------\n");

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
