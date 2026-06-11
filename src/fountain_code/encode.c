/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "utils.h"

static void vec_zero(vec_t* v, int n_words) {
  for (int i = 0; i < n_words; i++) v->w[i] = 0;
}

static void vec_set(vec_t* v, int bit) {
  v->w[bit / 64] |= (uint64_t)1 << (bit % 64);
}

packet_t encode_packet(int id, const block_t* blocks, int n, int m, int n_words) {
  packet_t pkt;
  pkt.id = id;
  vec_zero(&pkt.selector, n_words);
  memset(&pkt.data, 0, sizeof(pkt.data));

  for (int i = 0; i < m; i++) {
    int j = (int)(((uint64_t)(unsigned int)rand() + 314159311ULL * (uint64_t)(unsigned int)rand()) % (uint64_t)n);
    vec_set(&pkt.selector, j);
  }

  for (int j = 0; j < n; j++) {
    if (vec_test(&pkt.selector, j)) {
      data_xor(&pkt.data, &blocks[j]);
    }
  }

  return pkt;
}
