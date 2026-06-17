/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>
#include <time.h>

#include "rand128.h"
#include "rand64.h"
#include "time.h"
#include "utils.h"

void seed64_time(void) {
  uint64_t seed = ((uint64_t)(uint32_t)clock() << 32) | (uint64_t)(uint32_t)time(NULL);
  srand64(fmix64(seed));
}

void seed128_time(void) {
  uint64_t t = (uint64_t)(uint32_t)time(NULL);
  uint64_t c = (uint64_t)(uint32_t)clock();

  uint128_t s;
  s.lo = fmix64(t | (c << 32));
  s.hi = fmix64(c | (t << 32));

  srand128(s);
}
