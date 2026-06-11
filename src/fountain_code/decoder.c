/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <string.h>

#include "decoder.h"
#include "utils.h"

static void vec_xor(vec_t* dst, const vec_t* src, int n_words) {
  for (int i = 0; i < n_words; i++) dst->w[i] ^= src->w[i];
}

static int vec_lsb(const vec_t* v, int n_words) {
  for (int i = 0; i < n_words; i++) {
    if (0 != v->w[i]) {
      return 64 * i + ctz64(v->w[i]);
    }
  }
  return -1;
}

void decoder_init(decoder_t* dec, int n) {
  memset(dec, 0, sizeof(*dec));
  dec->n = n;
  dec->n_words = (n + 63) / 64;
  dec->remaining = n;
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

void decoder_solve(decoder_t* dec, block_t* out_blocks) {
  int n = dec->n;
  int nw = dec->n_words;

  for (int i = n - 1; i >= 0; i--) {
    if (!dec->pivot_present[i]) continue;
    for (int j = i + 1; j < n; j++) {
      if (!dec->pivot_present[j]) continue;
      if (!vec_test(&dec->pivot_sel[i], j)) continue;
      vec_xor(&dec->pivot_sel[i], &dec->pivot_sel[j], nw);
      data_xor(&dec->pivot_data[i], &dec->pivot_data[j]);
    }
  }

  for (int i = 0; i < n; i++) {
    if (dec->pivot_present[i]) {
      out_blocks[i] = dec->pivot_data[i];
    } else {
      memset(&out_blocks[i], 0, sizeof(block_t));
    }
  }
}
