/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <string.h>

#include "rand64.h"
#include "state.h"
#include "state_snapshot.h"

void rand64_save(rand64_state_t* out) {
  if (!prng_initialized) srand64(1); /* ensure PRNG is initialized */

  memcpy(out->table, prng_state, sizeof(prng_state));
  out->fptr_idx = (int)(prng_fptr - prng_state);
  out->rptr_idx = (int)(prng_rptr - prng_state);
  out->initialized = prng_initialized;
  out->seed = prng_seed;
}

void rand64_load(const rand64_state_t* in) {
  memcpy(prng_state, in->table, sizeof(prng_state));
  prng_fptr = prng_state + in->fptr_idx;
  prng_rptr = prng_state + in->rptr_idx;
  prng_initialized = in->initialized;
  prng_seed = in->seed;
}

void rand64_state_wipe(rand64_state_t* s) {
  volatile uint64_t* p = s->table;
  for (int i = 0; i < RAND64_DEG; i++) p[i] = 0;
  s->fptr_idx = 0;
  s->rptr_idx = 0;
  s->initialized = 0;
  s->seed = 0;
}
