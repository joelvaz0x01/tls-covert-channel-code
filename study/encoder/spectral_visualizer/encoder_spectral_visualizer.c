/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#define USE_SYSTEM_RANDOM 0 /* make results reproducible */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cbprng/cbprng.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <utils/encoder.h>
#include <utils/file.h>
#include <utils/utils.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "[-] Usage: %s <input> [dim] [n]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char* filename = argv[1];
  const char* dim = "2d";
  size_t n_samples = 1000;

  if (argc > 2) dim = argv[2];
  if (argc > 3) n_samples = (size_t)atol(argv[3]);

  int is_3d = (strcmp(dim, "3d") == 0);

  uint64_t n = calculate_n(filename);
  if (n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    exit(EXIT_FAILURE);
  }

  if (0 == open_file(filename, 1)) {
    fprintf(stderr, "[-] could not open %s\n", filename);
    exit(EXIT_FAILURE);
  }

  uint64_t m = generate_m(n);
  init_cbprng();

  static const int N_FIELDS = 2 + BLOCK_WORDS;

  double* x = malloc(n_samples * sizeof(double));
  if (!x) {
    fprintf(stderr, "[-] malloc failed\n");
    close_files();
    exit(EXIT_FAILURE);
  }

  size_t collected = 0;
  for (uint64_t id = 0; collected < n_samples; id++) {
    uint64_t n_words = init_program(n, m);
    while (0 != dec->remaining && collected < n_samples) {
      uint64_t seed = generate_cbprng(&cbprng, counter_value);
      uint64_t k = generate_k(m, seed, n, k_list);

      for (uint64_t i = 0; i < k; i++)
        if (-1 == read_file_part(filename, k_list[i], &buffer[i])) {
          fprintf(stderr, "[-] read_file_part failed: %lu\n", k_list[i]);
          close_files();
          finalize_program();
          free(x);
          exit(EXIT_FAILURE);
        }

      tls_mod_rand_t mod_rand = modified_random_field(id, &seed, m, n, n_words);

      double acc = (double)mod_rand.cbprng / (double)UINT64_MAX;
      for (int i = 0; i < BLOCK_WORDS - 1; i++)
        acc += (double)mod_rand.fountain_code.w[i] / (double)UINT64_MAX;
      acc += (double)(mod_rand.fountain_code.w[BLOCK_WORDS - 1] & 0xFFFFFFFF) / (double)UINT32_MAX;
      acc += (double)mod_rand.hash / (double)UINT32_MAX;
      x[collected] = acc / (double)N_FIELDS;

      collected++;
      counter_value++;
    }
    finalize_program();
  }

  close_files();

  if (is_3d) {
    write_dat_3d("encoder_spectral_data_3d.dat", x, n_samples);
    write_m_3d("encoder_spectral_plot_3d.m", "encoder_spectral_data_3d.dat", "encoder_spectral_3d.pdf", n_samples, "Encoder Spectral Test - Full Modified Random Field (3D)");
    printf("Run: octave --no-gui encoder_spectral_plot_3d.m\n");
  } else {
    write_dat_2d("encoder_spectral_data_2d.dat", x, n_samples);
    write_m_2d("encoder_spectral_plot_2d.m", "encoder_spectral_data_2d.dat", "encoder_spectral_2d.pdf", n_samples, "Encoder Spectral Test - Full Modified Random Field (2D)");
    printf("Run: octave --no-gui encoder_spectral_plot_2d.m\n");
  }

  free(x);
  return 0;
}
