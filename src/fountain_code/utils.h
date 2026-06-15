/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_UTILS_H
#define FOUNTAIN_CODE_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "settings.h"

/**
 * Returns the number of trailing zeros in a 64-bit word.
 * The behavior is undefined if value is 0.
 *
 * @param value The 64-bit word.
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
 * @struct vec_t
 * Represents a vector used in Gaussian elimination over GF(2).
 *
 * @var w Array of word-sized selector vectors.
 */
typedef struct {
  uint64_t w[VEC_WORDS];
} vec_t;

/**
 * @struct block_t
 * Represents a source block or encoded data block.
 *
 * @var w Array of word-sized data.
 */
typedef struct {
  uint64_t w[BLOCK_WORDS];
} block_t;

/**
 * @struct packet_t
 * Represents a fountain-code packet.
 *
 * @var id Packet ID.
 * @var selector Selector vector indicating which source blocks are included.
 * @var data Packet data (XOR of selected source blocks).
 */
typedef struct {
  int id;
  vec_t selector;
  block_t data;
} packet_t;

/**
 * @struct decoder_t
 * Represents the decoder state for Gaussian elimination over GF(2).
 *
 * @var n Number of source blocks.
 * @var n_words Number of word-sized selector vectors.
 * @var remaining Number of blocks still needed.
 * @var pivot_present Whether each block has a pivot.
 * @var pivot_sel Pivot selector for each block.
 * @var pivot_data Pivot data for each block.
 */
typedef struct {
  uint64_t n;
  uint64_t n_words;
  uint64_t remaining;
  bool pivot_present[MAX_BLOCKS];
  vec_t pivot_sel[MAX_BLOCKS];
  block_t pivot_data[MAX_BLOCKS];
} decoder_t;

/**
 * Tests the given bit in the vector.
 *
 * @param v Pointer to the vector.
 * @param bit The bit index to test.
 * @return true if the bit is set, false otherwise.
 */
static inline bool vec_test(const vec_t* v, uint64_t bit) {
  return (v->w[bit / 64] >> (bit % 64)) & 1;
}

/**
 * Sets a bit in a vector to 1.
 *
 * @param v Pointer to the vector.
 * @param bit The bit index to set.
 */
static inline void vec_set(vec_t* v, uint64_t bit) {
  v->w[bit / 64] |= (uint64_t)1 << (bit % 64);
}

/**
 * Performs XOR operation on two blocks of FC_BLOCK_SIZE bits.
 *
 * @param dst Pointer to the destination block.
 * @param src Pointer to the source block.
 */
void data_xor(block_t* dst, const block_t* src);

/**
 * Performs XOR operation on two vectors of n_words words.
 *
 * @param dst Pointer to the destination vector.
 * @param src Pointer to the source vector.
 * @param n_words Number of words to XOR.
 */
void vec_xor(vec_t* dst, const vec_t* src, uint64_t n_words);

/**
 * Initializes the decoder.
 *
 * @param dec Pointer to the decoder structure to initialize.
 * @param n Number of source blocks.
 */
void decoder_init(decoder_t* dec, uint64_t n);

#endif /* FOUNTAIN_CODE_UTILS_H */
