/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>

#include <rand/rand128.h>

#include <uint128/uint128.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  size_t n = 1000 + 2;
  if (argc > 1) n = (size_t)atol(argv[1]) + 2;

  uint128_t seed = U128(42, 0);
  srand128(seed);

  generator_t g;
  pseudo_random_generator(&g);

  double* x = malloc(n * sizeof(double));
  if (!x) {
    fprintf(stderr, "malloc failed\n");
    return 1;
  }

  for (mask_t counter = 0; counter < n; counter++) {
    x[counter] = (double)generate_cbprng(&g, counter) / (double)UINT64_MAX;
  }

  write_dat_2d("cbprng_spectral_data_2d.dat", x, n);
  write_dat_3d("cbprng_spectral_data_3d.dat", x, n);
  write_m_2d3d("cbprng_spectral_plot.m", "cbprng_spectral_data_2d.dat", "cbprng_spectral_data_3d.dat", "cbprng_spectral.png", n, "Visual Spectral Test - CBPRNG (2D)", "Visual Spectral Test - CBPRNG (3D)");
  printf("Run: octave --no-gui cbprng_spectral_plot.m\n");

  free(x);
  return 0;
}
