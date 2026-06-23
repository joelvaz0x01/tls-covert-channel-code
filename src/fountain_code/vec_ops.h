/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * __AVX2__ and __SSE2__ optimizations made with AI assistance
 */

#ifndef FOUNTAIN_CODE_VEC_OPS_H
#define FOUNTAIN_CODE_VEC_OPS_H

#include <stdint.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <immintrin.h>
#else
#include <string.h>
#endif

#include "utils.h"

static inline int ctz64(uint64_t value) {
#ifdef _MSC_VER
  unsigned long index;
  _BitScanForward64(&index, value);
  return (int)index;
#else
  return __builtin_ctzll((unsigned long long)value);
#endif
}

static inline bool vec_test(const vec_t* v, uint64_t bit) {
  return (v->w[bit / 64] >> (bit % 64)) & 1;
}

static inline void vec_set(vec_t* v, uint64_t bit) {
  v->w[bit / 64] |= (uint64_t)1 << (bit % 64);
}

static inline void vec_xor(vec_t* dst, const vec_t* src, uint64_t n_words) {
#if defined(__AVX2__)
  uint64_t i = 0;
  for (; i + 4 <= n_words; i += 4) {
    _mm256_storeu_si256((__m256i*)(dst->w + i), _mm256_xor_si256(_mm256_loadu_si256((const __m256i*)(dst->w + i)), _mm256_loadu_si256((const __m256i*)(src->w + i))));
  }
  for (; i < n_words; i++) dst->w[i] ^= src->w[i];
#elif defined(__SSE2__)
  uint64_t i = 0;
  for (; i + 2 <= n_words; i += 2) {
    _mm_store_si128((__m128i*)(dst->w + i), _mm_xor_si128(_mm_load_si128((const __m128i*)(dst->w + i)), _mm_load_si128((const __m128i*)(src->w + i))));
  }
  if (i < n_words) dst->w[i] ^= src->w[i];
#else
  for (uint64_t i = 0; i < n_words; i++) dst->w[i] ^= src->w[i];
#endif
}

static inline void data_xor(block_t* dst, const block_t* src) {
#if defined(__SSE2__)
  _mm_storeu_si128((__m128i*)dst, _mm_xor_si128(_mm_loadu_si128((const __m128i*)dst), _mm_loadu_si128((const __m128i*)src)));
  dst->w[2] ^= src->w[2];
#else
  for (uint64_t i = 0; i < BLOCK_WORDS; i++) dst->w[i] ^= src->w[i];
#endif
}

static inline void vec_zero(vec_t* v, uint64_t n_words) {
#if defined(__AVX2__)
  __m256i z = _mm256_setzero_si256();
  uint64_t i = 0;
  for (; i + 4 <= n_words; i += 4)
    _mm256_storeu_si256((__m256i*)(v->w + i), z);
  for (; i < n_words; i++) v->w[i] = 0;
#elif defined(__SSE2__)
  __m128i z = _mm_setzero_si128();
  uint64_t i = 0;
  for (; i + 2 <= n_words; i += 2)
    _mm_store_si128((__m128i*)(v->w + i), z);
  for (; i < n_words; i++) v->w[i] = 0;
#else
  memset(v->w, 0, n_words * sizeof(uint64_t));
#endif
}

static inline int64_t vec_lsb(const vec_t* v, uint64_t n_words) {
#if defined(__AVX2__)
  __m256i z = _mm256_setzero_si256();
  uint64_t i = 0;
  for (; i + 4 <= n_words; i += 4) {
    int m = _mm256_movemask_epi8(
      _mm256_cmpeq_epi64(_mm256_loadu_si256((const __m256i*)(v->w + i)), z)
    );
    if (m != -1) {
      for (uint64_t j = i; j < i + 4; j++)
        if (v->w[j]) return (int64_t)(64 * j + (uint64_t)ctz64(v->w[j]));
    }
  }
  for (; i < n_words; i++)
    if (v->w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
#elif defined(__SSE2__)
  __m128i z = _mm_setzero_si128();
  uint64_t i = 0;
  for (; i + 2 <= n_words; i += 2) {
    int m = _mm_movemask_epi8(
      _mm_cmpeq_epi64(_mm_load_si128((const __m128i*)(v->w + i)), z)
    );
    if (m != 0xFFFF) {
      if (v->w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
      if (v->w[i + 1]) return (int64_t)(64 * (i + 1) + (uint64_t)ctz64(v->w[i + 1]));
    }
  }
  if (i < n_words && v->w[i])
    return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
#else
  for (uint64_t i = 0; i < n_words; i++)
    if (v->w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
#endif
  return -1;
}

#endif /* FOUNTAIN_CODE_VEC_OPS_H */
