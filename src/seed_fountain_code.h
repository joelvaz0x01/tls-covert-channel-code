/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef SEED_FOUNTAIN_CODE_H
#define SEED_FOUNTAIN_CODE_H

#include <rand64/state_snapshot.h>

extern const rand64_seed_t fc_seed;

#define FC_SEED (&fc_seed)

/**
 * Initialize new seed and starts new state snapshot.
 */
void fc_seed_init(void);

/**
 * Save seed and state snapshot.
 */
void fc_seed_save(void);

/**
 * Restore seed and state from snapshot.
 */
void fc_seed_restore(void);

/**
 * Wipe seed and state snapshot.
 */
void fc_seed_wipe(void);

#endif /* SEED_FOUNTAIN_CODE_H */
