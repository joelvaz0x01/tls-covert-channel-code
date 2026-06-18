/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_UINT128_OPS_H
#define LCG_MULT_FINDER_UINT128_OPS_H

#include "uint128_t.h"

/**
 * Adds two 128-bit unsigned integers, handling carry correctly.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a + b.
 */
__device__ __host__ __forceinline__ uint128_t add128(uint128_t a, uint128_t b) {
  uint128_t c;
  c.lo = a.lo + b.lo;
  c.hi = a.hi + b.hi + (c.lo < a.lo ? 1 : 0);
  return c;
}

/**
 * Subtracts two 128-bit unsigned integers, handling borrow correctly.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a - b.
 */
__device__ __host__ __forceinline__ uint128_t sub128(uint128_t a, uint128_t b) {
  uint128_t c;
  c.lo = a.lo - b.lo;
  c.hi = a.hi - b.hi - (a.lo < b.lo ? 1 : 0);
  return c;
}

/**
 * Negates a 128-bit unsigned integer (two's complement).
 *
 * @param a The 128-bit unsigned integer to negate.
 * @return The negated value of a.
 */
__device__ __host__ __forceinline__ uint128_t neg128(uint128_t a) {
  uint128_t res;
  res.lo = ~a.lo + 1;
  res.hi = ~a.hi + (res.lo == 0 ? 1 : 0);
  return res;
}

/**
 * Returns true if a >= b for 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return True if a >= b, false otherwise.
 */
__device__ __host__ __forceinline__ bool gte128(uint128_t a, uint128_t b) {
  if (a.hi > b.hi) return true;
  if (a.hi == b.hi && a.lo >= b.lo) return true;
  return false;
}

/**
 * Returns true if a 128-bit unsigned integer is zero.
 *
 * @param a The 128-bit unsigned integer to check.
 * @return True if a is zero, false otherwise.
 */
__device__ __host__ __forceinline__ bool isZero128(uint128_t a) {
  return (a.lo == 0 && a.hi == 0);
}

#endif /* LCG_MULT_FINDER_UINT128_OPS_H */
