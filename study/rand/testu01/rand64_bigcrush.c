/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>
#include <stdio.h>

#include <rand/rand64.h>

#include "bbattery.h"
#include "unif01.h"
#include "utils.h"

#define BIGCRUSH_OUTPUT_FILE "rand64_bigcrush.txt"

int main(void) {
  printf("--- TestU01 Big Crush: rand64 64-bit LCG (128-bit internal state) ---\n\n");

  FILE* log = freopen(BIGCRUSH_OUTPUT_FILE, "w", stdout);
  if (NULL == log) {
    fprintf(stderr, "Error: could not open %s for writing\n", BIGCRUSH_OUTPUT_FILE);
    return 1;
  }

  srand64(42);

  rand_fn = rand64;

  unif01_Gen* gen = unif01_CreateExternGenBits("rand64 64-bit LCG (128-bit state, Upper 64 bits)", lcg_next);
  bbattery_BigCrush(gen);

  unif01_DeleteExternGenBits(gen);
  fclose(log);

  return 0;
}
