/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_UINT128_OPS_H
#define LCG_MULT_FINDER_UINT128_OPS_H

#include "uint128_t.h"

#if LCG_UINT128_HAS_NATIVE

/**
 * Adds two 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a + b.
 */
__device__ __host__ __forceinline__ uint128_t add128(uint128_t a, uint128_t b) {
  return a + b;
}

/**
 * Subtracts two 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a - b.
 */
__device__ __host__ __forceinline__ uint128_t sub128(uint128_t a, uint128_t b) {
  return a - b;
}

/**
 * Negates a 128-bit unsigned integer.
 *
 * @param a The 128-bit unsigned integer to negate.
 * @return The negated value of a.
 */
__device__ __host__ __forceinline__ uint128_t neg128(uint128_t a) {
  return -a;
}

/**
 * Returns true if a >= b for 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return True if a is greater than or equal to b, false otherwise.
 */
__device__ __host__ __forceinline__ bool gte128(uint128_t a, uint128_t b) {
  return a >= b;
}

/**
 * Returns true if the given 128-bit unsigned integer is zero.
 *
 * @param a The 128-bit unsigned integer to check.
 * @return True if a is zero, false otherwise.
 */
__device__ __host__ __forceinline__ bool isZero128(uint128_t a) {
  return a == 0;
}

#else

/**
 * Adds two 128-bit unsigned integers, handling carry correctly.
 *
 * This code was been generated with AI assistance.
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
 * This code was been generated with AI assistance.
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
 * This code was been generated with AI assistance.
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
 * This code was been generated with AI assistance.
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
 * This code was been generated with AI assistance.
 *
 * @param a The 128-bit unsigned integer to check.
 * @return True if a is zero, false otherwise.
 */
__device__ __host__ __forceinline__ bool isZero128(uint128_t a) {
  return (a.lo == 0 && a.hi == 0);
}

#endif

#endif /* LCG_MULT_FINDER_UINT128_OPS_H */
