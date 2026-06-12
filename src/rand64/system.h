/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_SYSTEM_H
#define RAND64_SYSTEM_H

#include <stdint.h>

/**
 * Provides a seed for the 64-bit PRNG using system entropy.
 *
 * @return the 64-bit seed
 */
uint64_t seed64_system(void);

#endif /* RAND64_SYSTEM_H */
