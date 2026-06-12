/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_TIME_H
#define RAND64_TIME_H

#include <stdint.h>

/**
 * Provides a seed for the 64-bit PRNG using wall-clock and CPU time.
 *
 * @return the 64-bit seed
 */
uint64_t seed64_time(void);

#endif /* RAND64_TIME_H */
