/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_RAND128_H
#define RAND_RAND128_H

#include <uint128/uint128.h>

/* Maximum value returned by rand128() -> 2^64 - 1 */
#define RAND128_MAX UINT64_C(0xFFFFFFFFFFFFFFFF)

/**
 * Seeds the 128-bit LCG with the given value.
 *
 * If seed is all-zero it is replaced with {1, 0}, matching the glibc srand() convention.
 *
 * @param seed 128-bit seed value.
 */
void srand128(uint128_t seed);

/**
 * Returns the next pseudo-random value as a 64-bit integer.
 *
 * Internally advances a 128-bit LCG state; the upper 64 bits of the
 * 128-bit state are returned (proven to pass BigCrush).
 *
 * If srand128() has never been called, it will seed automatically with {1, 0}.
 *
 * @return A pseudo-random value in [0, RAND128_MAX].
 */
uint64_t rand128(void);

/**
 * Returns a random uint64_t between min and max, inclusive.
 *
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A pseudo-random value in [min, max].
 */
uint64_t rand128_between(uint64_t min, uint64_t max);

#endif /* RAND_RAND128_H */
