/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmp_uint64(const void* a, const void* b) {
  uint64_t x = *(const uint64_t*)a;
  uint64_t y = *(const uint64_t*)b;
  if (x < y) return -1;
  if (x > y) return 1;
  return 0;
}

#include <rand/rand128.h>

#include "settings.h"
#include "utils.h"
#include "vec_ops.h"

int decoder_init(decoder_t* dec, const uint64_t n) {
  dec->n = n;
  dec->n_words = (n + 63) / 64;
  dec->remaining = n;
  dec->pivot_present = NULL;
  dec->pivot_sel = NULL;
  dec->pivot_data = NULL;
  dec->scratch_sel = NULL;

  dec->pivot_present = calloc((size_t)n, sizeof(bool));
  dec->pivot_sel = calloc((size_t)n, sizeof(vec_t));
  dec->pivot_data = calloc((size_t)n, sizeof(block_t));
  dec->scratch_sel = calloc(1, sizeof(vec_t));

  if (NULL == dec->pivot_present || NULL == dec->pivot_sel || NULL == dec->pivot_data || NULL == dec->scratch_sel) {
    decoder_destroy(dec);
    return -1;
  }

  return 0;
}

void decoder_destroy(decoder_t* dec) {
  if (NULL == dec) return;

  free(dec->pivot_present);
  free(dec->pivot_sel);
  free(dec->pivot_data);
  free(dec->scratch_sel);

  dec->pivot_present = NULL;
  dec->pivot_sel = NULL;
  dec->pivot_data = NULL;
  dec->scratch_sel = NULL;

  free(dec);
  dec = NULL;
}

uint64_t generate_m(const uint64_t n) {
  uint64_t m = (uint64_t)round(ALPHA * log((double)n) + EULER);
  if (m >= n) m = n - 1;
  if (m == 0) m = 1;
  return m;
}

uint64_t generate_k(const uint64_t m, const uint64_t seed, const uint64_t n, uint64_t* out) {
  static vec_t* seen = NULL;
  if (NULL == seen) {
    seen = calloc(1, sizeof(vec_t));
    if (NULL == seen) return 0;
  }

  uint64_t modified[256];
  uint64_t mod_count = 0;
  uint64_t count = 0;

  srand128(seed);
  for (uint64_t i = 0; i < m; i++) {
    uint64_t v = rand128_between(0, n - 1);
    if (!vec_test(seen, v)) {
      vec_set(seen, v);
      if (mod_count < 256) modified[mod_count++] = v / 64;
      out[count++] = v;
    }
  }

  for (uint64_t i = 0; i < mod_count; i++)
    seen->w[modified[i]] = 0;

  qsort(out, count, sizeof(uint64_t), cmp_uint64);
  return count;
}
