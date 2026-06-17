/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <math.h>
#include <stdint.h>
#include <string.h>

#include <rand/rand64.h>

#include "settings.h"
#include "utils.h"

void data_xor(block_t* dst, const block_t* src) {
  for (uint64_t i = 0; i < BLOCK_WORDS; i++) {
    dst->w[i] ^= src->w[i];
  }
}

void vec_xor(vec_t* dst, const vec_t* src, uint64_t n_words) {
  for (uint64_t i = 0; i < n_words; i++) dst->w[i] ^= src->w[i];
}

void decoder_init(decoder_t* dec, uint64_t n) {
  memset(dec, 0, sizeof(*dec));
  dec->n = n;
  dec->n_words = (n + 63) / 64;
  dec->remaining = n;
}

uint64_t generate_k(uint64_t m, uint64_t seed, uint64_t n, uint64_t* out) {
  srand64(seed);

  vec_t seen = {0};
  uint64_t count = 0;

  for (uint64_t i = 0; i < m; i++) {
    uint64_t v = rand64_between(0, n - 1);
    if (!vec_test(&seen, v)) {
      vec_set(&seen, v);
      out[count++] = v;
    }
  }
  return count;
}

uint64_t generate_m(uint64_t n) {
  uint64_t m = (uint64_t)round(ALPHA * log((double)n) + EULER);
  if (m == 0) m = 1;
  if (m >= n) m = n - 1;
  return m;
}
