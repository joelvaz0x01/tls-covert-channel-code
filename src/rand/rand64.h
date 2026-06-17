/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_RAND64_H
#define RAND_RAND64_H

#include <stdint.h>

/* Maximum value returned by rand64() -> 2^64 - 1 */
#define RAND64_MAX UINT64_C(0xFFFFFFFFFFFFFFFF)

/**
 * Seeds the 64-bit PRNG with the given value.
 *
 * @param seed 64-bit seed value.
 */
void srand64(uint64_t seed);

/**
 * Returns the next pseudo-random 64-bit integer in [0, RAND64_MAX].
 *
 * If srand64() has never been called, it will seed automatically with 1.
 *
 * @return A pseudo-random value in [0, RAND64_MAX].
 */
uint64_t rand64(void);

/**
 * Returns a random uint64_t between min and max, inclusive.
 *
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A pseudo-random value in [min, max].
 */
uint64_t rand64_between(uint64_t min, uint64_t max);

#endif /* RAND_RAND64_H */
