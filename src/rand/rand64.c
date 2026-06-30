/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * 64-bit API wrapping a 128-bit LCG.
 * Partially adapted from glibc.
 */

#include <stdbool.h>
#include <stdint.h>

#include <uint128/uint128.h>

#include "rand64.h"
#include "settings.h"
#include "utils.h"

static uint128_t state = U128(0, 0);
static bool initialized = false;

void srand64(uint64_t s) {
  if (!s) s = 1; /* avoid zero seed */

  state = U128(s, fmix64(s));
  initialized = true;
}

uint64_t rand64(void) {
  if (!initialized) {
    srand64(1);
  }

  state = lcg128(state, LCG_MUL, LCG_ADD);
  return U128_HI(state); /* return upper 64 bits */
}
