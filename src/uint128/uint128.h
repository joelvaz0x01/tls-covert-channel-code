/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UINT128_H
#define UINT128_H

#if defined(__SIZEOF_INT128__)

#define UINT128_NATIVE 1

__extension__ typedef unsigned __int128 uint128_t;

#define U128(lo, hi) (((uint128_t)(lo)) | ((uint128_t)(hi) << 64))
#define U128_LO(v)   ((uint64_t)(v))
#define U128_HI(v)   ((uint64_t)((v) >> 64))

/**
 * Adds two 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a + b.
 */
static inline uint128_t add128(uint128_t a, uint128_t b) {
  return a + b;
}

/**
 * Multiplies two 128-bit unsigned integers.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a * b.
 */
static inline uint128_t mul128(uint128_t a, uint128_t b) {
  return a * b;
}

#else

#include <stdint.h>

#define UINT128_FALLBACK 1

#define U128(lo, hi)     ((uint128_t){(lo), (hi)})
#define U128_LO(v)       ((v).lo)
#define U128_HI(v)       ((v).hi)

/**
 * @struct uint128_t
 * Represents a 128-bit unsigned integer using two 64-bit unsigned integers.
 *
 * @var lo The lower 64 bits of the 128-bit integer.
 * @var hi The higher 64 bits of the 128-bit integer.
 */
typedef struct {
  uint64_t lo;
  uint64_t hi;
} uint128_t;

/**
 * Adds two 128-bit values with carry propagation from lo to hi.
 *
 * This code was been generated with AI assistance.
 *
 * @param a The first 128-bit unsigned integer.
 * @param b The second 128-bit unsigned integer.
 * @return The result of a + b.
 */
static inline uint128_t add128(uint128_t a, uint128_t b) {
  uint128_t r;
  r.lo = a.lo + b.lo;
  r.hi = a.hi + b.hi + (r.lo < a.lo ? 1 : 0);
  return r;
}

/**
 * Returns the upper 64 bits of the full 128-bit product a * b.
 *
 * This code was been generated with AI assistance.
 *
 * @param a First 64-bit operand.
 * @param b Second 64-bit operand.
 * @return Upper 64 bits of a * b.
 */
static inline uint64_t mulhi64(uint64_t a, uint64_t b) {
  uint32_t a_lo = (uint32_t)a;         /* lower 32 bits of 'a' */
  uint32_t a_hi = (uint32_t)(a >> 32); /* upper 32 bits of 'a' */
  uint32_t b_lo = (uint32_t)b;         /* lower 32 bits of 'b' */
  uint32_t b_hi = (uint32_t)(b >> 32); /* upper 32 bits of 'b' */

  uint64_t p0 = (uint64_t)a_lo * b_lo; /* bits [63:0]   - multiply the lower 32 bits of 'a' by the lower 32 bits of 'b'      */
  uint64_t p1 = (uint64_t)a_lo * b_hi; /* bits [95:32]  - multiply the lower 32 bits of 'a' by the upper 32 bits of 'b'      */
  uint64_t p2 = (uint64_t)a_hi * b_lo; /* bits [95:32]  - multiply the upper 32 bits of 'a' by the lower 32 bits of 'b'      */
  uint64_t p3 = (uint64_t)a_hi * b_hi; /* bits [127:64] - multiply the upper 32 bits of 'a' by the upper 32 bits of 'b'      */

  uint64_t mid = (p0 >> 32) + (uint32_t)p1 + (uint32_t)p2; /* mid 32 bits of the product */
  return p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
}

/**
 * Multiplies two 128-bit values, returning the low 128 bits of the product.
 *
 * This code was been generated with AI assistance.
 *
 * @param a First operand.
 * @param b Second operand.
 * @return (a * b) mod 2^128.
 */
static inline uint128_t mul128(uint128_t a, uint128_t b) {
  uint128_t r;
  r.lo = a.lo * b.lo;

  r.hi = a.hi * b.lo            /* cross-product a.hi * b.lo */
         + a.lo * b.hi          /* cross-product a.lo * b.hi */
         + mulhi64(a.lo, b.lo); /* carry from a.lo * b.lo    */

  return r;
}

#endif

#endif /* UINT128_H */
