/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rand/rand128.h>
#include <uint128/uint128.h>

static void write_dat_2d(const char* path, double* x, size_t n) {
  FILE* f = fopen(path, "w");
  if (!f) {
    perror(path);
    exit(1);
  }
  for (size_t i = 0; i < n - 1; i++) {
    fprintf(f, "%.15e\t%.15e\n", x[i], x[i + 1]);
  }
  fclose(f);
}

static void write_dat_3d(const char* path, double* x, size_t n) {
  FILE* f = fopen(path, "w");
  if (!f) {
    perror(path);
    exit(1);
  }
  for (size_t i = 0; i < n - 2; i++) {
    fprintf(f, "%.15e\t%.15e\t%.15e\n", x[i], x[i + 1], x[i + 2]);
  }
  fclose(f);
}

static void write_m_2d(const char* m_path, const char* dat_name, const char* pdf_name) {
  FILE* f = fopen(m_path, "w");
  if (!f) {
    perror(m_path);
    exit(1);
  }
  fprintf(f,
          "data = load('%s');\n"
          "x = data(:,1);\n"
          "y = data(:,2);\n"
          "\n"
          "figure('visible', 'off');\n"
          "set(gcf, 'PaperUnits', 'centimeters');\n"
          "set(gcf, 'PaperSize', [24 16]);\n"
          "set(gcf, 'PaperPosition', [1 1 22 14]);\n"
          "plot(x, y, 'b.', 'MarkerSize', 8);\n"
          "title('Visual Spectral Test - rand128 (2D)');\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "xlim([0 1]); ylim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1); zticks(0:0.1:1);\n"
          "print('-dpdf', '%s');\n",
          dat_name,
          pdf_name);
  fclose(f);
}

static void write_m_3d(const char* m_path, const char* dat_name, const char* pdf_name) {
  FILE* f = fopen(m_path, "w");
  if (!f) {
    perror(m_path);
    exit(1);
  }
  fprintf(f,
          "data = load('%s');\n"
          "x = data(:,1);\n"
          "y = data(:,2);\n"
          "z = data(:,3);\n"
          "\n"
          "figure('visible', 'off');\n"
          "set(gcf, 'PaperUnits', 'centimeters');\n"
          "set(gcf, 'PaperSize', [24 16]);\n"
          "set(gcf, 'PaperPosition', [1 1 22 14]);\n"
          "plot3(x, y, z, 'b.', 'MarkerSize', 8);\n"
          "title('Visual Spectral Test - rand128 (3D)');\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "zlabel('x_{i+2}');\n"
          "xlim([0 1]); ylim([0 1]); zlim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1); zticks(0:0.1:1);\n"
          "view(60, 30);\n"
          "print('-dpdf', '%s');\n",
          dat_name,
          pdf_name);
  fclose(f);
}

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
    x[i] = (double)rand128() / (double)UINT64_MAX;
  }

  if (is_3d) {
    write_dat_3d("rand128_spectral_data_3d.dat", x, n);
    write_m_3d("rand128_spectral_plot_3d.m", "rand128_spectral_data_3d.dat", "rand128_spectral_3d.pdf");
    printf("Run: octave --no-gui rand128_spectral_plot_3d.m\n");
  } else {
    write_dat_2d("rand128_spectral_data_2d.dat", x, n);
    write_m_2d("rand128_spectral_plot_2d.m", "rand128_spectral_data_2d.dat", "rand128_spectral_2d.pdf");
    printf("Run: octave --no-gui rand128_spectral_plot_2d.m\n");
  }

  free(x);
  return 0;
}
