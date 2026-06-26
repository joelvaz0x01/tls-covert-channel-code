/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <rand/rand64.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  size_t n = 1000 + 2;

  if (argc > 1) n = (size_t)atol(argv[1]) + 2;

  srand64(42);

  double* x = malloc(n * sizeof(double));
  if (!x) {
    fprintf(stderr, "malloc failed\n");
    return 1;
  }

  for (size_t i = 0; i < n; i++) {
    x[i] = (double)rand64() / (double)RAND64_MAX;
  }

  write_dat_2d("rand64_spectral_data_2d.dat", x, n);
  write_dat_3d("rand64_spectral_data_3d.dat", x, n);
  write_m_2d3d("rand64_spectral_plot.m", "rand64_spectral_data_2d.dat", "rand64_spectral_data_3d.dat", "rand64_spectral.png", n, "Visual Spectral Test - rand64 (2D)", "Visual Spectral Test - rand64 (3D)");
  printf("Run: octave --no-gui rand64_spectral_plot.m\n");

  free(x);
  return 0;
}
