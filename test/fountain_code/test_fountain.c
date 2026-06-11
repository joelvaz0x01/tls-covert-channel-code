#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fountain_code/decoder.h>
#include <fountain_code/encoder.h>

/**
 * Performs a complete encode-decode-verify cycle for a given number of source blocks.
 *
 * @param n Number of source blocks to test.
 */
void run_test(int n) {
  printf("Testing with n = %4d blocks...  ", n);

  if (n < 2 || n > MAX_BLOCKS) {
    fprintf(stderr, "Error: invalid n = %d (must be between 2 and %d)\n", n, MAX_BLOCKS);
    assert(0);
  }

  block_t* src = malloc((size_t)n * sizeof(block_t));
  assert(src != NULL);

  /* fill blocks with some pseudo-random data */
  unsigned char* src_bytes = (unsigned char*)src;
  for (size_t i = 0; i < (size_t)n * sizeof(block_t); i++) {
    src_bytes[i] = (unsigned char)(rand() % 256);
  }

  int m = (int)round(2.0 * log((double)n) + EULER);
  if (m < 1) m = 1;
  if (m >= n) m = n - 1;

  /* decode blocks on-the-fly */
  decoder_t* dec = calloc(1, sizeof(decoder_t));
  assert(NULL != dec);
  decoder_init(dec, n);

  int n_words = (n + 63) / 64;
  int total_sent = 0;

  /* keep sending packets until the decoder has found enough pivots to solve the system */
  while (dec->remaining > 0) {
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
    fprintf(stderr, "FAILURE: Reconstructed data does not match original for n=%d\n", n);
    assert(match);
  }

  printf("SUCCESS (sent %4d packets)\n", total_sent);

  free(src);
  free(out);
  free(dec);
}

int main(void) {
  srand((unsigned int)time(NULL));
  printf("\nStarting Fountain Code Implementation Tests...\n");
  printf("-------------------------------------------------------------\n");

  /* test a variety of sizes */
  run_test(2);
  run_test(5);
  run_test(10);
  run_test(32);
  run_test(64);
  run_test(100);
  run_test(256);
  run_test(512);
  run_test(MAX_BLOCKS); /* maximum supported */

  printf("-------------------------------------------------------------\n");
  printf("All tests passed successfully!\n\n");

  return 0;
}
