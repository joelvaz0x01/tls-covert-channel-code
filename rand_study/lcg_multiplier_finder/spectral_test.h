/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_SPECTRAL_TEST_H
#define LCG_MULT_FINDER_SPECTRAL_TEST_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "uint128_t.h"

/**
 * Evaluates the spectral test for a given multiplier lambda in 128-bit precision.
 *
 * @param lambda The multiplier to evaluate.
 * @return true if the spectral test passes, false otherwise.
 */
bool passes_higher_dimensions_128(uint128_t lambda);

/**
 * Evaluates the spectral test for a given multiplier lambda in 64-bit precision.
 *
 * @param lambda The multiplier to evaluate.
 * @return true if the spectral test passes, false otherwise.
 */
bool passes_higher_dimensions_64(unsigned long long lambda);

#ifdef __cplusplus
}
#endif

#endif  // LCG_MULT_FINDER_SPECTRAL_TEST_H
