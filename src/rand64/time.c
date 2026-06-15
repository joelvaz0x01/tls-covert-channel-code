/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>
#include <time.h>

#include "rand64.h"
#include "time.h"

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

void seed64_time(void) {
  /*
   * lower 32 bits: current time
   * upper 32 bits: number of clock ticks since process start
   */
  uint64_t seed = ((uint64_t)(uint32_t)clock() << 32) | (uint64_t)(uint32_t)time(NULL);

  /* spread entropy across all bits */
  srand64(fmix64(seed));
}
