/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_UTILS_H
#define RAND_UTILS_H

#include <stdint.h>

#include <uint128/uint128.h>

/**
 * Finalizes a 64-bit value using MurmurHash3 finalizer mix.
 *
 * @param k The value to finalize.
 * @return The finalized value.
 */
static inline uint64_t fmix64(uint64_t k) {
  k ^= k >> 33;
  k *= UINT64_C(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= UINT64_C(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

/**
 * 128-bit LCG recurrence:
 *   - state = (mul * state + add) mod 2^128.
 *
 * @param state Current 128-bit state.
 * @param mul 128-bit multiplier.
 * @param add 128-bit addend.
 * @return The next 128-bit state.
 */
static inline uint128_t lcg128(uint128_t state, uint128_t mul, uint128_t add) {
  return add128(mul128(mul, state), add);
}

#endif /* RAND_UTILS_H */
