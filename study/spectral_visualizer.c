/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "spectral_visualizer.h"

void write_dat_2d(const char* path, double* x, size_t n) {
  FILE* f = fopen(path, "w");
  if (!f) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < n - 1; i++) {
    fprintf(f, "%.15e\t%.15e\n", x[i], x[i + 1]);
  }
  fclose(f);
}

void write_dat_3d(const char* path, double* x, size_t n) {
  FILE* f = fopen(path, "w");
  if (!f) {
    perror(path);
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < n - 2; i++) {
    fprintf(f, "%.15e\t%.15e\t%.15e\n", x[i], x[i + 1], x[i + 2]);
  }
  fclose(f);
}

void write_m_2d(const char* m_path, const char* dat_name, const char* pdf_name, size_t n, const char* title) {
  FILE* f = fopen(m_path, "w");
  if (!f) {
    perror(m_path);
    exit(EXIT_FAILURE);
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
          "title({'%s', 'first %zu values'});\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "xlim([0 1]); ylim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1); zticks(0:0.1:1);\n"
          "print('-dpdf', '%s');\n",
          dat_name,
          title,
          n,
          pdf_name);
  fclose(f);
}

void write_m_3d(const char* m_path, const char* dat_name, const char* pdf_name, size_t n, const char* title) {
  FILE* f = fopen(m_path, "w");
  if (!f) {
    perror(m_path);
    exit(EXIT_FAILURE);
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
          "title({'%s', 'first %zu values'});\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "zlabel('x_{i+2}');\n"
          "xlim([0 1]); ylim([0 1]); zlim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1); zticks(0:0.1:1);\n"
          "view(60, 30);\n"
          "print('-dpdf', '%s');\n",
          dat_name,
          title,
          n,
          pdf_name);
  fclose(f);
}
