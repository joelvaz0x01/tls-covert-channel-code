/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "utils.h"

/**
 * Sets all words of a vector to zero.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words to set to zero.
 */
static void vec_zero(vec_t* v, const uint64_t n_words) {
  for (uint64_t i = 0; i < n_words; i++) v->w[i] = 0;
}

/**
 * Returns the index of the least significant bit set in a vector.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words to check.
 * @return The index of the least significant bit set, or -1 if none is set.
 */
static int64_t vec_lsb(const vec_t* v, const uint64_t n_words) {
  for (uint64_t i = 0; i < n_words; i++)
    if (0 != v->w[i])
      return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
  return -1;
}

packet_t encode_packet(const int id, const uint64_t seed, const block_t* blocks, const uint64_t n, const uint64_t m, const uint64_t n_words) {
  packet_t pkt;
  pkt.id = id;
  vec_zero(&pkt.selector, n_words);
  memset(&pkt.data, 0, sizeof(pkt.data));

  uint64_t* indices = malloc(m * sizeof(uint64_t));
  if (!indices) return pkt;
  uint64_t k = generate_k(m, seed, n, indices);
  for (uint64_t i = 0; i < k; i++) {
    vec_set(&pkt.selector, indices[i]);
  }
  free(indices);

  for (uint64_t j = 0; j < n; j++) {
    if (vec_test(&pkt.selector, j)) {
      data_xor(&pkt.data, &blocks[j]);
    }
  }

  return pkt;
}

bool decoder_feed(decoder_t* dec, const packet_t* pkt) {
  vec_t sel = pkt->selector;
  block_t dat = pkt->data;

  for (;;) {
    int64_t i = vec_lsb(&sel, dec->n_words);
    if (i < 0) return false;

    if (!dec->pivot_present[i]) {
      dec->pivot_present[i] = true;
      dec->pivot_sel[i] = sel;
      dec->pivot_data[i] = dat;
      dec->remaining--;
      return true;
    }
    vec_xor(&sel, &dec->pivot_sel[i], dec->n_words);
    data_xor(&dat, &dec->pivot_data[i]);
  }
}
