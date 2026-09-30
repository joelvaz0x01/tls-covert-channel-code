/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef STUDY_SPECTRAL_VISUALIZER_H
#define STUDY_SPECTRAL_VISUALIZER_H

#include <stddef.h>

/**
 * Write 2D consecutive-pair spectral test data.
 * @param path output .dat file path
 * @param x array of n uniform [0,1] samples
 * @param n number of elements in x
 *
 * Writes n-1 rows: each row is "x[i]\tx[i+1]".
 */
void write_dat_2d(const char* path, const double* x, size_t n);

/**
 * Write 3D consecutive-triple spectral test data.
 * @param path output .dat file path
 * @param x array of n uniform [0,1] samples
 * @param n number of elements in x
 *
 * Writes n-2 rows: each row is "x[i]\tx[i+1]\tx[i+2]".
 */
void write_dat_3d(const char* path, const double* x, size_t n);

/**
 * Generate an Octave/MATLAB script with 2D and 3D scatter plots side by side.
 * @param m_path output .m script path
 * @param dat_name_2d name/path of the 2D .dat file to load
 * @param dat_name_3d name/path of the 3D .dat file to load
 * @param pdf_name name/path of the output PDF
 * @param n number of samples (shown in plot titles)
 * @param title_2d 2D subplot title string
 * @param title_3d 3D subplot title string
 */
void write_m_2d3d(const char* m_path, const char* dat_name_2d, const char* dat_name_3d, const char* pdf_name, size_t n, const char* title_2d, const char* title_3d);

#endif /* STUDY_SPECTRAL_VISUALIZER_H */
