/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Advanced Vector Extensions (AVX2) and
 * Streaming SIMD Extensions 4 (SSE4)
 * optimizations made with AI assistance
 */

#ifndef FOUNTAIN_CODE_VEC_OPS_H
#define FOUNTAIN_CODE_VEC_OPS_H

#include <stdint.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <immintrin.h>
#else
#include <string.h>
#endif

#if defined(__SSE2__) && !defined(__AVX2__)
#include "cpu.h"
#endif

#include "utils.h"

/**
 * Count the trailing zeros in a 64-bit integer.
 *
 * @param value The 64-bit integer to count trailing zeros in.
 * @return The number of trailing zeros.
 */
static inline int ctz64(uint64_t value) {
#ifdef _MSC_VER
  unsigned long index;
  _BitScanForward64(&index, value);
  return (int)index;
#else
  return __builtin_ctzll((unsigned long long)value);
#endif
}

/**
 * Test if a bit is set in a vector.
 *
 * @param v Pointer to the vector.
 * @param bit The index of the bit to test.
 * @return True if the bit is set, false otherwise.
 */
static inline bool vec_test(const vec_t* v, uint64_t bit) {
  return (v->w[bit / 64] >> (bit % 64)) & 1;
}

/**
 * Set a bit in a vector to 1.
 *
 * @param v Pointer to the vector.
 * @param bit The index of the bit to set.
 */
static inline void vec_set(vec_t* v, uint64_t bit) {
  v->w[bit / 64] |= (uint64_t)1 << (bit % 64);
}

/**
 * Perform XOR operation on two vectors.
 *
 * @param dst Pointer to the destination vector.
 * @param src Pointer to the source vector.
 * @param n_words Number of words in the vectors.
 */
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

/**
 * Perform XOR operation on two Fountain Code data.
 *
 * @param dst Pointer to the destination block.
 * @param src Pointer to the source block.
 */
static inline void data_xor(block_t* dst, const block_t* src) {
#if defined(__SSE2__)
  uint64_t i = 0;
  for (; i + 2 <= BLOCK_WORDS; i += 2)
    _mm_storeu_si128((__m128i*)(dst->w + i), _mm_xor_si128(_mm_loadu_si128((const __m128i*)(dst->w + i)), _mm_loadu_si128((const __m128i*)(src->w + i))));
  for (; i < BLOCK_WORDS; i++) dst->w[i] ^= src->w[i];
#else
  for (uint64_t i = 0; i < BLOCK_WORDS; i++) dst->w[i] ^= src->w[i];
#endif
}

/**
 * Zero out the words of a vector.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words in the vector.
 */
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

/**
 * Find the least significant bit (LSB) of a vector
 * using SSE4.2 instructions.
 *
 * @param w Pointer to the vector words.
 * @param n_words Number of words in the vector.
 * @return The index of the LSB, or -1 if the vector is all zero.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((target("sse4.2")))
#endif
static inline int64_t vec_lsb_sse4_2(const uint64_t* w, uint64_t n_words) {
  __m128i z = _mm_setzero_si128();
  uint64_t i = 0;
  for (; i + 2 <= n_words; i += 2) {
    int m = _mm_movemask_epi8(
      _mm_cmpeq_epi64(_mm_load_si128((const __m128i*)(w + i)), z)
    );
    if (m != 0xFFFF) {
      if (w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(w[i]));
      if (w[i + 1]) return (int64_t)(64 * (i + 1) + (uint64_t)ctz64(w[i + 1]));
    }
  }
  if (i < n_words && w[i])
    return (int64_t)(64 * i + (uint64_t)ctz64(w[i]));
  return -1;
}

/**
 * Main function that finds the least significant bit (LSB)
 * of a vector.
 *
 * @param v Pointer to the vector.
 * @param n_words Number of words in the vector.
 * @return The index of the LSB, or -1 if the vector is all zero.
 */
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
  if (cpu_has_sse4_2())
    return vec_lsb_sse4_2(v->w, n_words);
  for (uint64_t i = 0; i < n_words; i++)
    if (v->w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
#else
  for (uint64_t i = 0; i < n_words; i++)
    if (v->w[i]) return (int64_t)(64 * i + (uint64_t)ctz64(v->w[i]));
#endif
  return -1;
}

#endif /* FOUNTAIN_CODE_VEC_OPS_H */
