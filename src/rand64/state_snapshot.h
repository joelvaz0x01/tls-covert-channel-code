/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_STATE_SNAPSHOT_H
#define RAND64_STATE_SNAPSHOT_H

#include <stdint.h>

#include "state.h"

/**
 * @struct rand64_seed_t
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
 * @struct rand64_state_t
 * Complete snapshot of the 64-bit PRNG state.
 *
 * @var table Copy of the 31-word state table.
 * @var fptr_idx Front-pointer index into table.
 * @var rptr_idx Rear-pointer index into table.
 * @var initialized Non-zero when the PRNG has been seeded.
 * @var seed Last seed passed to srand64().
 */
typedef struct {
  uint64_t table[RAND64_DEG];
  int fptr_idx;
  int rptr_idx;
  int initialized;
  uint64_t seed;
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
