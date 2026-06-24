/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rand/rand128.h>
#include <uint128/uint128.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  const char* dim = "2d";
  size_t n = 1000;

  if (argc > 1) dim = argv[1];
  if (argc > 2) n = (size_t)atol(argv[2]);

  int is_3d = (strcmp(dim, "3d") == 0);

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

  if (is_3d) {
    write_dat_3d("rand128_spectral_data_3d.dat", x, n);
    write_m_3d("rand128_spectral_plot_3d.m", "rand128_spectral_data_3d.dat", "rand128_spectral_3d.pdf", n, "Visual Spectral Test - rand128 (3D)");
    printf("Run: octave --no-gui rand128_spectral_plot_3d.m\n");
  } else {
    write_dat_2d("rand128_spectral_data_2d.dat", x, n);
    write_m_2d("rand128_spectral_plot_2d.m", "rand128_spectral_data_2d.dat", "rand128_spectral_2d.pdf", n, "Visual Spectral Test - rand128 (2D)");
    printf("Run: octave --no-gui rand128_spectral_plot_2d.m\n");
  }

  free(x);
  return 0;
}
