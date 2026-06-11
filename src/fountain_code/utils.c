/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include "utils.h"

int vec_test(const vec_t* v, int bit) {
  return (int)((v->w[bit / 64] >> (bit % 64)) & 1);
}

void data_xor(block_t* dst, const block_t* src) {
  for (int i = 0; i < BLOCK_WORDS; i++) {
    dst->w[i] ^= src->w[i];
  }
}
