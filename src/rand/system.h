/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_SYSTEM_H
#define RAND_SYSTEM_H

/**
 * Seeds the 64-bit PRNG using system entropy.
 *
 * Reads 8 bytes from BCryptGenRandom (Windows) or /dev/urandom (POSIX).
 * Falls back to seed64_time() if the entropy source is unavailable.
 */
void seed64_system(void);

/**
 * Seeds the 128-bit LCG using system entropy.
 *
 * Reads 16 bytes from BCryptGenRandom (Windows) or /dev/urandom (POSIX).
 * Falls back to seed128_time() if the entropy source is unavailable.
 */
void seed128_system(void);

#endif /* RAND_SYSTEM_H */
