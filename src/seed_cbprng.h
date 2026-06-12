/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef SEED_CBPRNG_H
#define SEED_CBPRNG_H

#include <rand64/state_snapshot.h>

extern const rand64_seed_t cbprng_seed;

#define CBPRNG_SEED (&cbprng_seed)

/**
 * Initialize new seed and starts new state snapshot.
 */
void cbprng_seed_init(void);

/**
 * Save seed and state snapshot.
 */
void cbprng_seed_save(void);

/**
 * Restore seed and state from snapshot.
 */
void cbprng_seed_restore(void);

/**
 * Wipe seed and state snapshot.
 */
void cbprng_seed_wipe(void);

#endif /* SEED_CBPRNG_H */
