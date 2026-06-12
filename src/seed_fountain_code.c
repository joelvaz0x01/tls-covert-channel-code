/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <rand64/rand64.h>
#include <rand64/system.h>

#include "seed_fountain_code.h"

static rand64_state_t fc_state;

const rand64_seed_t fc_seed = {
  .init = fc_seed_init,
  .save = fc_seed_save,
  .restore = fc_seed_restore,
  .wipe = fc_seed_wipe,
};

void fc_seed_init(void) {
  srand64(seed64_system());
  rand64_save(&fc_state);
}

void fc_seed_save(void) {
  rand64_save(&fc_state);
}

void fc_seed_restore(void) {
  rand64_load(&fc_state);
}

void fc_seed_wipe(void) {
  rand64_state_wipe(&fc_state);
}
