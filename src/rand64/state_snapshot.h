/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_STATE_SNAPSHOT_H
#define RAND64_STATE_SNAPSHOT_H

#include <stdint.h>

#include "state.h"

/**
 * @struct seed_t
 * Represents a seed for the random number generator.
 *
 * @var init Initializes the seed.
 * @var save Saves the seed and state snapshot.
 * @var restore Restores the seed and state from snapshot.
 * @var wipe Wipes the seed and state snapshot.
 */
typedef struct {
  void (*init)(void);
  void (*save)(void);
  void (*restore)(void);
  void (*wipe)(void);
} rand64_seed_t;

/**
 * Complete snapshot of the 64-bit PRNG state.
 *
 * Capture with rand64_save(); restore with rand64_load().
 * Storing this struct preserves not just the seed but also the exact
 * position inside the state table, so that rand64() continues from the
 * same point after a restore.
 */
typedef struct {
  uint64_t table[RAND64_DEG]; /* copy of the 31-word state table          */
  int fptr_idx;               /* front-pointer index into table           */
  int rptr_idx;               /* rear-pointer index into table            */
  int initialized;            /* non-zero when the PRNG has been seeded   */
  uint64_t seed;              /* last seed passed to srand64()            */
} rand64_state_t;

/**
 * Captures the complete PRNG state.
 *
 * If the generator is not initialized, will be initialized with
 * seed 1 (default seed) and saved a snapshot.
 *
 * @param out Destination snapshot; must not be NULL.
 */
void rand64_save(rand64_state_t* out);

/**
 * Restores the PRNG to the state previously captured by rand64_save().
 *
 * @param in Source snapshot; must not be NULL.
 */
void rand64_load(const rand64_state_t* in);

/**
 * Securely zeroes a rand64_state_t snapshot.
 *
 * @param s Snapshot to zero; must not be NULL.
 */
void rand64_state_wipe(rand64_state_t* s);

#endif /* RAND64_STATE_SNAPSHOT_H */
