/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_H
#define RAND64_H

#include <stdint.h>

#define RAND64_MAX UINT64_C(0x7FFFFFFFFFFFFFFF) /**< Maximum value returned by rand64() -> 2^63 - 1 */

/**
 * Seeds the 64-bit PRNG with the given value.
 *
 * If seed is 0 it is replaced with 1, matching the glibc srand() behavior.
 *
 * After seeding the state table is warmed up with DEG * 10 discard steps
 * to remove initial correlations from the linear fill.
 *
 * @param PRNG seed 64-bit value.
 */
void srand64(uint64_t seed);

/**
 * Returns the next pseudo-random 64-bit integer in [0, RAND64_MAX].
 *
 * The least-significant bit is discarded following the glibc random() behavior.
 *
 * If srand64() has never been called, it will seed automatically with 1.
 *
 * @return  A pseudo-random value in [0, RAND64_MAX].
 */
uint64_t rand64(void);

#endif /* RAND64_H */
