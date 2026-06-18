/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_GPU_ENGINE_H
#define LCG_MULT_FINDER_GPU_ENGINE_H

#include <stdio.h>

static inline void format_and_print_time(double total_seconds) {
  int hours = (int)(total_seconds / 3600);
  int mins = (int)((total_seconds - hours * 3600) / 60);
  int secs = (int)(total_seconds - hours * 3600 - mins * 60);
  printf("%d hours, %d minutes, %d seconds", hours, mins, secs);
}

/**
 * Runs the 64-bit GPU-accelerated search for optimal LCG multipliers.
 * Candidates pass a fast GPU filter and are validated via CPU spectral test.
 */
void run_64_bit_search(void);

/**
 * Runs the 128-bit GPU-accelerated search for optimal LCG multipliers.
 * Candidates pass a fast GPU filter and are validated via CPU spectral test.
 */
void run_128_bit_search(void);

#endif /* LCG_MULT_FINDER_GPU_ENGINE_H */
