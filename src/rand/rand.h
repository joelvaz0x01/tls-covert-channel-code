/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_H
#define RAND_H

/* By default, uses the system random number generator is used */
#ifndef RANDOM_ALGORITHM
#define RANDOM_ALGORITHM 0
#endif

/**
 * Select the right PRNG seeding method based on the RANDOM_ALGORITHM configuration.
 *
 * If RANDOM_ALGORITHM = 0, uses system random number generator
 * If RANDOM_ALGORITHM = 1, uses time-based seeding.
 */
void seed_prng(void);

#endif /* RAND_H */
