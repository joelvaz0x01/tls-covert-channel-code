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
void write_dat_2d(const char* path, double* x, size_t n);

/**
 * Write 3D consecutive-triple spectral test data.
 * @param path output .dat file path
 * @param x array of n uniform [0,1] samples
 * @param n number of elements in x
 *
 * Writes n-2 rows: each row is "x[i]\tx[i+1]\tx[i+2]".
 */
void write_dat_3d(const char* path, double* x, size_t n);

/**
 * Generate an Octave/MATLAB script for a 2D scatter plot.
 * @param m_path output .m script path
 * @param dat_name name/path of the .dat file to load
 * @param pdf_name name/path of the output PDF
 * @param n number of samples (shown in plot title)
 * @param title plot title string (e.g. "Visual Spectral Test - CBPRNG (2D)")
 */
void write_m_2d(const char* m_path, const char* dat_name, const char* pdf_name, size_t n, const char* title);

/**
 * Generate an Octave/MATLAB script for a 3D scatter plot.
 * @param m_path output .m script path
 * @param dat_name name/path of the .dat file to load
 * @param pdf_name name/path of the output PDF
 * @param n number of samples (shown in plot title)
 * @param title plot title string (e.g. "Visual Spectral Test - CBPRNG (3D)")
 */
void write_m_3d(const char* m_path, const char* dat_name, const char* pdf_name, size_t n, const char* title);

#endif /* STUDY_SPECTRAL_VISUALIZER_H */
