/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <rand64/rand64.h>
#include <rand64/system.h>

#include "seed_cbprng.h"

static rand64_state_t cbprng_state;

const rand64_seed_t cbprng_seed = {
  .init = cbprng_seed_init,
  .save = cbprng_seed_save,
  .restore = cbprng_seed_restore,
  .wipe = cbprng_seed_wipe,
};

void cbprng_seed_init(void) {
  srand64(seed64_system());
  rand64_save(&cbprng_state);
}

void cbprng_seed_save(void) {
  rand64_save(&cbprng_state);
}

void cbprng_seed_restore(void) {
  rand64_load(&cbprng_state);
}

void cbprng_seed_wipe(void) {
  rand64_state_wipe(&cbprng_state);
}
