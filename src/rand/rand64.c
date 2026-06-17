/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * 64-bit API wrapping a 128-bit LCG.
 *
 * Partially adapted from glibc.
 */

#include <stdint.h>

#include <uint128/uint128.h>

#include "rand64.h"
#include "settings.h"
#include "utils.h"

static uint128_t state = {0, 0};
static int initialized = 0;

void srand64(uint64_t s) {
  if (!s) s = 1; /* avoid zero seed */

  state.lo = s;
  state.hi = fmix64(s);
  initialized = 1;
}

uint64_t rand64(void) {
  if (!initialized) {
    srand64(1);
  }

  static const uint128_t mul = {LCG_MUL_LO, LCG_MUL_HI};
  static const uint128_t add = {LCG_ADD, 0};
  state = lcg128(state, mul, add);

  return state.hi; /* return upper 64 bits */
}

uint64_t rand64_between(uint64_t min, uint64_t max) {
  return min + rand64() % (max - min + 1);
}
