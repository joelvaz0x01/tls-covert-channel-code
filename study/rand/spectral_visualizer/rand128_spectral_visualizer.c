/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <rand/rand128.h>

#include <uint128/uint128.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  size_t n = 1000 + 2;

  if (argc > 1) n = (size_t)atol(argv[1]) + 2;

  uint128_t seed = U128(42, 0);
  srand128(seed);

  double* x = malloc(n * sizeof(double));
  if (!x) {
    fprintf(stderr, "malloc failed\n");
    return 1;
  }

  for (size_t i = 0; i < n; i++) {
    x[i] = (double)rand128() / (double)RAND128_MAX;
  }

  write_dat_2d("rand128_spectral_data_2d.dat", x, n);
  write_dat_3d("rand128_spectral_data_3d.dat", x, n);
  write_m_2d3d("rand128_spectral_plot.m", "rand128_spectral_data_2d.dat", "rand128_spectral_data_3d.dat", "rand128_spectral.png", n, "Visual Spectral Test - rand128 (2D)", "Visual Spectral Test - rand128 (3D)");
  printf("Run: octave --no-gui rand128_spectral_plot.m\n");

  free(x);
  return 0;
}
