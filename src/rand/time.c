/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdlib.h>
#include <time.h>

#include "time.h"

void seed_prng_time(void) {
  srand((unsigned int)time(NULL));
}
