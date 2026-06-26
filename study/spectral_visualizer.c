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
  for (size_t i = 0; i < n; i++) {
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
  for (size_t i = 0; i < n; i++) {
    fprintf(f, "%.15e\t%.15e\t%.15e\n", x[i], x[i + 1], x[i + 2]);
  }
  fclose(f);
}

void write_m_2d3d(const char* m_path, const char* dat_name_2d, const char* dat_name_3d, const char* pdf_name, size_t n, const char* title_2d, const char* title_3d) {
  FILE* f = fopen(m_path, "w");
  if (!f) {
    perror(m_path);
    exit(EXIT_FAILURE);
  }
  fprintf(f,
          "data2 = load('%s');\n"
          "x2 = data2(:,1);\n"
          "y2 = data2(:,2);\n"
          "data3 = load('%s');\n"
          "x3 = data3(:,1);\n"
          "y3 = data3(:,2);\n"
          "z3 = data3(:,3);\n"
          "\n"
          "figure('visible', 'off');\n"
          "set(gcf, 'PaperUnits', 'centimeters');\n"
          "set(gcf, 'PaperSize', [35 18]);\n"
          "set(gcf, 'PaperPosition', [0.1 0.1 34.8 17.8]);\n"
          "\n"
          "axes('Position', [0.06 0.08 0.42 0.84]);\n"
          "plot(x2, y2, 'b.', 'MarkerSize', 5);\n"
          "pbaspect([1 1 1]);\n"
          "title({'%s', 'first %zu values'});\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "xlim([0 1]); ylim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1);\n"
          "\n"
          "axes('Position', [0.52 0.08 0.42 0.84]);\n"
          "plot3(x3, y3, z3, 'b.', 'MarkerSize', 5);\n"
          "pbaspect([1.125 1 1]);\n"
          "title({'%s', 'first %zu values'});\n"
          "grid on;\n"
          "xlabel('x_{i}');\n"
          "ylabel('x_{i+1}');\n"
          "zlabel('x_{i+2}');\n"
          "xlim([0 1]); ylim([0 1]); zlim([0 1]);\n"
          "xticks(0:0.1:1); yticks(0:0.1:1); zticks(0:0.1:1);\n"
          "view(60, 30);\n"
          "\n"
          "print('-dpng', '%s');\n",
          dat_name_2d,
          dat_name_3d,
          title_2d,
          n,
          title_3d,
          n,
          pdf_name);
  fclose(f);
}
