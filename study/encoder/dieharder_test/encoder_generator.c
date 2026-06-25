/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#define USE_SYSTEM_RANDOM 0 /* make results reproducible */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <utils/encoder.h>
#include <utils/file.h>
#include <utils/utils.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "[-] Usage: %s <input>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  char* filename = argv[1];

  uint64_t n = calculate_n(filename);
  if (n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    exit(EXIT_FAILURE);
  }

  if (0 == open_file(filename, 1)) {
    fprintf(stderr, "[-] could not open %s\n", filename);
    exit(EXIT_FAILURE);
  }

  uint64_t m = generate_m(n);
  init_cbprng();

  for (uint64_t id = 0;; id++) {
    uint64_t n_words = init_program(n, m);
    while (0 != dec->remaining) {
      uint64_t seed = generate_cbprng(&cbprng, counter_value);
      uint64_t k = generate_k(m, seed, n, k_list);

      for (uint64_t i = 0; i < k; i++)
        if (-1 == read_file_part(filename, k_list[i], &buffer[i])) {
          fprintf(stderr, "[-] read_file_part failed: %lu\n", k_list[i]);
          close_files();
          finalize_program();
          exit(EXIT_FAILURE);
        }

      tls_mod_rand_t mod_rand = modified_random_field(id, &seed, m, n, n_words);
      write_fountain(mod_rand, stdout);
      counter_value++;
    }
    finalize_program();
  }

  close_files();

  return 0;
}
