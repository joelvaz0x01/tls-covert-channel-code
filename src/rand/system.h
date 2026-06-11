/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_SYSTEM_H
#define RAND_SYSTEM_H

/**
 * Seeds the standard C PRNG using system entropy (/dev/urandom or BCryptGenRandom).
 */
void seed_prng_system(void);

#endif /* RAND_SYSTEM_H */
