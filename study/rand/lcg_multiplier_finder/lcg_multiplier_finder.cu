/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "gpu_engine.h"

int main(int argc, char* argv[]) {
  int bits = 128;

  if (argc > 1) {
    bits = atoi(argv[1]);
    if (bits != 64 && bits != 128) {
      printf("Error: Unsupported bit size. Use 64 or 128.\n");
      return 1;
    }
  }

  if (bits == 64)
    run_64_bit_search();
  else
    run_128_bit_search();

  return 0;
}
