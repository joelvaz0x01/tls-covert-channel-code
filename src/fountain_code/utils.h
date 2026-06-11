/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_UTILS_H
#define FOUNTAIN_CODE_UTILS_H

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
  return __builtin_ctzll(value);
#endif
}

/**
 * Tests the given bit in the vector.
 *
 * @param v Pointer to the vector.
 * @param bit The bit index to test.
 * @return 1 if the bit is set, 0 otherwise.
 */
int vec_test(const vec_t* v, int bit);

/**
 * Performs XOR operation on two blocks of FC_BLOCK_SIZE bits.
 *
 * @param dst Pointer to the destination block.
 * @param src Pointer to the source block.
 */
void data_xor(block_t* dst, const block_t* src);

#endif /* FOUNTAIN_CODE_UTILS_H */
