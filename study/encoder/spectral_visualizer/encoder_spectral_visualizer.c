/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#define USE_SYSTEM_RANDOM 0 /* make results reproducible */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>

#include <fountain_code/utils.h>

#include <utils/encoder.h>
#include <utils/file.h>
#include <utils/utils.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "[-] Usage: %s <input> [n]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char* filename = argv[1];
  size_t n_samples = 1000 + 2;

  if (argc > 2) n_samples = (size_t)atol(argv[2]) + 2;

  uint64_t n = calculate_n(filename, true);
  if (n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    exit(EXIT_FAILURE);
  }

  if (0 == open_file(filename, 1)) {
    fprintf(stderr, "[-] could not open %s\n", filename);
    exit(EXIT_FAILURE);
  }

  uint64_t m = generate_m(n);
  uint64_t n_words = init_program(n, m);

  init_cbprng();

  double* x = (double*)malloc(n_samples * sizeof(double));
  if (!x) {
    fprintf(stderr, "[-] malloc failed\n");
    close_files();
    exit(EXIT_FAILURE);
  }

  size_t collected = 0;
  uint64_t id = 0;

  while (collected < n_samples) {
    while (0 != dec->remaining && collected < n_samples) {
      uint64_t seed = generate_cbprng(&cbprng, counter_value);
      if (0 == construct_k(seed, m, n, n_words, filename)) {
        fprintf(stderr, "[-] construct_k failed\n");
        close_files();
        finalize_program();
        free(x);
        exit(EXIT_FAILURE);
      }

      tls_mod_rand_t mod_rand = modified_random_field(id, &seed, m, n, n_words, NULL);
      x[collected] = (double)mod_rand.enc_fc.w[0] / (double)UINT64_MAX;

      collected++;
      counter_value++;
    }
    n_words = reset_program(n);
    id++;
  }

  close_files();
  finalize_program();

  write_dat_2d("encoder_spectral_data_2d.dat", x, n_samples);
  write_dat_3d("encoder_spectral_data_3d.dat", x, n_samples);
  write_m_2d3d("encoder_spectral_plot.m", "encoder_spectral_data_2d.dat", "encoder_spectral_data_3d.dat", "encoder_spectral.png", n_samples - 2, "Encoder Spectral Test - Full Modified Random Field (2D)", "Encoder Spectral Test - Full Modified Random Field (3D)");
  printf("Run: octave --no-gui encoder_spectral_plot.m\n");

  free(x);
  return 0;
}
