/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <string.h>

#include <rand64/rand64.h>

#include "encoder.h"
#include "utils.h"

/**
 * Sets all words of a vector to zero.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words to set to zero.
 */
static void vec_zero(vec_t* v, int n_words) {
  for (int i = 0; i < n_words; i++) v->w[i] = 0;
}

/**
 * Sets a bit in a vector to 1.
 *
 * @param v Pointer to the vector.
 * @param bit The bit index to set.
 */
static void vec_set(vec_t* v, int bit) {
  v->w[bit / 64] |= (uint64_t)1 << (bit % 64);
}

/**
 * Returns the index of the least significant bit set in a vector.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words to check.
 * @return The index of the least significant bit set, or -1 if none is set.
 */
static int vec_lsb(const vec_t* v, int n_words) {
  for (int i = 0; i < n_words; i++) {
    if (0 != v->w[i]) {
      return 64 * i + ctz64(v->w[i]);
    }
  }
  return -1;
}

packet_t encode_packet(int id, const block_t* blocks, int n, int m, int n_words) {
  packet_t pkt;
  pkt.id = id;
  vec_zero(&pkt.selector, n_words);
  memset(&pkt.data, 0, sizeof(pkt.data));

  for (int i = 0; i < m; i++) {
    int j = (int)(((uint64_t)(unsigned int)rand64() + 314159311ULL * (uint64_t)(unsigned int)rand64()) % (uint64_t)n);
    vec_set(&pkt.selector, j);
  }

  for (int j = 0; j < n; j++) {
    if (vec_test(&pkt.selector, j)) {
      data_xor(&pkt.data, &blocks[j]);
    }
  }

  return pkt;
}

int decoder_feed(decoder_t* dec, const packet_t* pkt) {
  vec_t sel = pkt->selector;
  block_t dat = pkt->data;

  for (;;) {
    int i = vec_lsb(&sel, dec->n_words);
    if (i < 0) return 0;

    if (!dec->pivot_present[i]) {
      dec->pivot_present[i] = 1;
      dec->pivot_sel[i] = sel;
      dec->pivot_data[i] = dat;
      dec->remaining--;
      return 1;
    }
    vec_xor(&sel, &dec->pivot_sel[i], dec->n_words);
    data_xor(&dat, &dec->pivot_data[i]);
  }
}
