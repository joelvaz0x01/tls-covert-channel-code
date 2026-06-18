/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <NTL/LLL.h>
#include <NTL/ZZ.h>
#include <NTL/mat_ZZ.h>
#include <cstdio>

#include "spectral_test.h"

/**
 * Evaluates the spectral test for a given multiplier lambda.
 *
 * @param a The multiplier to evaluate.
 * @param m The modulus.
 * @param max_bits The maximum number of bits to consider.
 * @return true if the spectral test passes, false otherwise.
 */
static bool evaluate_spectral_test(NTL::ZZ a, NTL::ZZ m, int max_bits) {
  for (long d = 3; d <= 6; d++) {
    NTL::mat_ZZ B;
    B.SetDims(d, d);

    B[0][0] = m;
    NTL::ZZ a_pow = NTL::ZZ(1);
    for (long i = 1; i < d; ++i) {
      a_pow = (a_pow * a) % m;
      B[i][0] = -a_pow;
      B[i][i] = 1;
    }

    NTL::ZZ det2;
    NTL::LLL(det2, B, 99, 100);

    NTL::ZZ min_norm = NTL::to_ZZ(0);
    for (long j = 0; j < d; ++j) {
      min_norm += B[0][j] * B[0][j];
    }

    for (long i = 1; i < d; i++) {
      NTL::ZZ current_norm = NTL::to_ZZ(0);
      for (long j = 0; j < d; j++) {
        current_norm += B[i][j] * B[i][j];
      }
      if (current_norm < min_norm) {
        min_norm = current_norm;
      }
    }

    long actual_bits = NTL::NumBits(min_norm);

    // Max theoretical squared norm bits: (modulus_bits * 2) / d
    double theoretical_max_bits = (max_bits * 2.0) / d;

    if (actual_bits < (theoretical_max_bits * 0.85)) {
      printf("      -> Failed at dimension %ld (Too sparse)\n", d);
      return false;
    }
  }
  return true;
}

bool passes_higher_dimensions_128(uint128_t lambda) {
  NTL::ZZ a = (NTL::ZZ(lambda.hi) << 64) | NTL::ZZ(lambda.lo);
  NTL::ZZ m = NTL::ZZ(1) << 128;

  printf("\n[CPU 128] 2D Passed. Evaluating 0x%016llx%016llx in 3D-6D...\n", lambda.hi, lambda.lo);
  return evaluate_spectral_test(a, m, 128);
}

bool passes_higher_dimensions_64(unsigned long long lambda) {
  NTL::ZZ a = NTL::ZZ(lambda);
  NTL::ZZ m = NTL::ZZ(1) << 64;

  printf("\n[CPU 64] 2D Passed. Evaluating 0x%016llx in 3D-6D...\n", lambda);
  return evaluate_spectral_test(a, m, 64);
}
