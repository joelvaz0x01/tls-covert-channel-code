/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Pure 128-bit LCG implementation.
 *
 * Partially adapted from glibc.
 */

#include <stdbool.h>

#include <uint128/uint128.h>

#include "rand128.h"
#include "settings.h"
#include "utils.h"

static uint128_t state = U128(0, 0);
static uint128_t seed = U128(0, 0);
static bool initialized = false;

void srand128(uint128_t s) {
  if (!U128_LO(s) && !U128_HI(s)) s = U128(1, 0); /* avoid zero seed */

  seed = s;
  state = s;
  initialized = true;
}

uint64_t rand128(void) {
  if (!initialized) {
    uint128_t default_seed = U128(1, 0);
    srand128(default_seed);
  }

  state = lcg128(state, LCG_MUL, LCG_ADD);
  return U128_HI(state); /* return upper 64 bits */
}

uint64_t rand128_between(uint64_t min, uint64_t max) {
  return min + rand128() % (max - min + 1);
}
