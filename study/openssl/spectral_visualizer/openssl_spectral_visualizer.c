/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <study/spectral_visualizer.h>

int main(int argc, char* argv[]) {
  const char* dim = "2d";
  size_t n_samples = 1000;

  if (argc > 1) dim = argv[1];
  if (argc > 2) n_samples = (size_t)atol(argv[2]);

  int is_3d = (strcmp(dim, "3d") == 0);
  static const int N_FIELDS = 5;

  OSSL_LIB_CTX* ctx = OSSL_LIB_CTX_new();
  if (!ctx) {
    fprintf(stderr, "[-] OSSL_LIB_CTX_new failed\n");
    exit(EXIT_FAILURE);
  }

  unsigned char buf[32];
  double* x = malloc(n_samples * sizeof(double));
  if (!x) {
    fprintf(stderr, "[-] malloc failed\n");
    OSSL_LIB_CTX_free(ctx);
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < n_samples; i++) {
    if (1 != RAND_bytes_ex(ctx, buf, 32, 0)) {
      fprintf(stderr, "[-] RAND_bytes_ex failed at iteration %zu\n", i);
      OSSL_LIB_CTX_free(ctx);
      free(x);
      exit(EXIT_FAILURE);
    }

    uint64_t f0, f1, f2;
    uint32_t f3, f4;
    memcpy(&f0, buf, 8);
    memcpy(&f1, buf + 8, 8);
    memcpy(&f2, buf + 16, 8);
    memcpy(&f3, buf + 24, 4);
    memcpy(&f4, buf + 28, 4);

    double acc = (double)f0 / (double)UINT64_MAX;
    acc += (double)f1 / (double)UINT64_MAX;
    acc += (double)f2 / (double)UINT64_MAX;
    acc += (double)f3 / (double)UINT32_MAX;
    acc += (double)f4 / (double)UINT32_MAX;
    x[i] = acc / (double)N_FIELDS;
  }

  OSSL_LIB_CTX_free(ctx);

  if (is_3d) {
    write_dat_3d("openssl_spectral_data_3d.dat", x, n_samples);
    write_m_3d("openssl_spectral_plot_3d.m", "openssl_spectral_data_3d.dat", "openssl_spectral_3d.pdf", n_samples, "OpenSSL RAND\\_bytes\\_ex Spectral Test (3D)");
    printf("Run: octave --no-gui openssl_spectral_plot_3d.m\n");
  } else {
    write_dat_2d("openssl_spectral_data_2d.dat", x, n_samples);
    write_m_2d("openssl_spectral_plot_2d.m", "openssl_spectral_data_2d.dat", "openssl_spectral_2d.pdf", n_samples, "OpenSSL RAND\\_bytes\\_ex Spectral Test (2D)");
    printf("Run: octave --no-gui openssl_spectral_plot_2d.m\n");
  }

  free(x);
  return 0;
}
