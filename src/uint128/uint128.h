/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UINT128_H
#define UINT128_H

#include <stdint.h>

/**
 * @struct uint128_t
 * Portable 128-bit unsigned integer type using two 64-bit halves.
 * Compatible with platforms that do not provide __uint128_t (e.g. MSVC/Windows).
 *
 * @var lo Lower 64 bits.
 * @var hi Upper 64 bits.
 */
typedef struct {
  uint64_t lo;
  uint64_t hi;
} uint128_t;

/**
 * Adds two 128-bit values with carry propagation from lo to hi.
 *
 * @param a First operand.
 * @param b Second operand.
 * @return a + b as uint128_t.
 */
static inline uint128_t add128(uint128_t a, uint128_t b) {
  uint128_t r;
  r.lo = a.lo + b.lo;
  r.hi = a.hi + b.hi + (r.lo < a.lo); /* propagate carry */
  return r;
}

/**
 * Returns the upper 64 bits of the full 128-bit product a * b.
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

#endif /* UINT128_H */
