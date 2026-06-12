/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <string.h>

#include "utils.h"

int vec_test(const vec_t* v, int bit) {
  return (int)((v->w[bit / 64] >> (bit % 64)) & 1);
}

void data_xor(block_t* dst, const block_t* src) {
  for (int i = 0; i < BLOCK_WORDS; i++) {
    dst->w[i] ^= src->w[i];
  }
}

void vec_xor(vec_t* dst, const vec_t* src, int n_words) {
  for (int i = 0; i < n_words; i++) dst->w[i] ^= src->w[i];
}

void decoder_init(decoder_t* dec, int n) {
  memset(dec, 0, sizeof(*dec));
  dec->n = n;
  dec->n_words = (n + 63) / 64;
  dec->remaining = n;
}
