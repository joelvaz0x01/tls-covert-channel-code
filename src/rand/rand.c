/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include "rand.h"

#if RANDOM_ALGORITHM == 0
#include "system.h"
void seed_prng(void) {
  seed_prng_system();
}
#else
#include "time.h"
void seed_prng(void) {
  seed_prng_time();
}
#endif
