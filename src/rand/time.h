/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_TIME_H
#define RAND_TIME_H

/**
 * Seeds the 64-bit PRNG using wall-clock and CPU time.
 */
void seed64_time(void);

/**
 * Seeds the 128-bit LCG using wall-clock and CPU time.
 *
 * Derives two independent 64-bit values from time() and clock(),
 * runs each through MurmurHash3 fmix64, and uses them as {lo, hi}.
 */
void seed128_time(void);

#endif /* RAND_TIME_H */
