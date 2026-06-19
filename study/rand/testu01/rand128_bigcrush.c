/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>
#include <stdio.h>

#include <rand/rand128.h>
#include <uint128/uint128.h>

#include "bbattery.h"
#include "unif01.h"
#include "utils.h"

#define BIGCRUSH_OUTPUT_FILE "rand128_bigcrush.txt"

int main(void) {
  printf("--- TestU01 Big Crush: rand128 128-bit LCG ---\n\n");

  FILE* log = freopen(BIGCRUSH_OUTPUT_FILE, "w", stdout);
  if (NULL == log) {
    fprintf(stderr, "Error: could not open %s for writing\n", BIGCRUSH_OUTPUT_FILE);
    return 1;
  }

  uint128_t seed = U128(42, 0);
  srand128(seed);

  rand_fn = rand128;

  unif01_Gen* gen = unif01_CreateExternGenBits("rand128 128-bit LCG (Upper 64 bits)", lcg_next);
  bbattery_BigCrush(gen);

  unif01_DeleteExternGenBits(gen);
  fclose(log);

  return 0;
}
