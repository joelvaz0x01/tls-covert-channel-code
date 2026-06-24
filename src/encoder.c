/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <utils/file.h>
#include <utils/utils.h>

#include "encoder.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "[-] usage: %s <input> <output>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char* src_file = argv[1];
  if (0 == open_file(src_file, 1)) {
    fprintf(stderr, "[-] could not open %s\n", src_file);
    exit(EXIT_FAILURE);
  }

  char* dest_file = argv[2];
  if (0 == open_file(dest_file, 0)) {
    fprintf(stderr, "[-] could not open %s\n", dest_file);
    close_files();
    exit(EXIT_FAILURE);
  }

  uint64_t n = calculate_n(src_file);
  if (n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    exit(EXIT_FAILURE);
  }

  uint64_t m = generate_m(n);
  uint64_t n_words = init_program(n, m);

  init_cbprng();

  while (0 != dec->remaining) {
    uint64_t seed = generate_cbprng(&cbprng, counter_value);
    uint64_t k = generate_k(m, seed, n, k_list);

    for (uint64_t i = 0; i < k; i++) {
      if (-1 == read_file_part(src_file, k_list[i], &buffer[i])) {
        fprintf(stderr, "[-] read_file_part failed: %lu\n", k_list[i]);
        finalize_program();
        exit(EXIT_FAILURE);
      }
    }

    tls_mod_rand_t data = modified_random_field(0, &seed, m, n, n_words);
    save_encoder(dest_file, data);
    counter_value++;
  }

  close_files();
  finalize_program();

  return 0;
}
