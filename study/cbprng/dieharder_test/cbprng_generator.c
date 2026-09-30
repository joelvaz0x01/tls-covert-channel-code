/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>

#include <cbprng/cbprng.h>
#include <cbprng/settings.h>

#include <rand/rand128.h>

#include <uint128/uint128.h>

int main(void) {
  uint128_t seed = U128(42, 0);
  srand128(seed);

  generator_t g;
  pseudo_random_generator(&g);

  for (mask_t counter = 0;; counter++) {
    mask_t val = generate_cbprng(&g, counter);
    fwrite((void*)&val, sizeof(val), 1, stdout);
  }
}
