/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <NTL/LLL.h>
#include <NTL/ZZ.h>
#include <NTL/mat_ZZ.h>

#include <array>
#include <print>

#include "spectral_test.h"
#include "uint128_t.h"

#if defined(__SIZEOF_INT128__)

/**
 * Extracts the low 64 bits of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @return The low 64 bits of v.
 */
static inline uint64_t get_lo128(uint128_t v) {
  return (uint64_t)v;
}

/**
 * Extracts the high 64 bits of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @return The high 64 bits of v.
 */
static inline uint64_t get_hi128(uint128_t v) {
  return (uint64_t)(v >> 64);
}

#else

/**
 * Extracts the low 64 bits of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @return The low 64 bits of v.
 */
static inline uint64_t get_lo128(uint128_t v) {
  return v.lo;
}

/**
 * Extracts the high 64 bits of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @return The high 64 bits of v.
 */
static inline uint64_t get_hi128(uint128_t v) {
  return v.hi;
}

#endif

static long early_termination_check(const NTL::vec_ZZ&) {
  return 0;
}

/**
 * Evaluates the spectral test for a given multiplier lambda.
 *
 * This code was been generated with AI assistance.
 *
 * @param a The multiplier to evaluate.
 * @param m The modulus.
 * @param max_bits The maximum number of bits to consider.
 * @return true if the spectral test passes, false otherwise.
 */
static bool evaluate_spectral_test(NTL::ZZ a, NTL::ZZ m, int max_bits) {
  std::array<NTL::ZZ, 6> a_powers;
  a_powers[0] = NTL::to_ZZ(1);
  for (int i = 1; i < 6; i++) {
    a_powers[i] = (a_powers[i - 1] * a) % m;
  }

  for (long d = 3; d <= 6; d++) {
    NTL::mat_ZZ B;
    B.SetDims(d, d);

    B[0][0] = m;
    for (long i = 1; i < d; ++i) {
      B[i][0] = -a_powers[i];
      B[i][i] = 1;
    }

    NTL::LLL_FP(B, 0.99, 0, early_termination_check);

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

    double theoretical_max_bits = (max_bits * 2.0) / d;
    auto min_norm_bits = (long)(theoretical_max_bits * 0.85);

    if (actual_bits < min_norm_bits) {
      std::print("      -> Failed at dimension %ld (Too sparse, got %ld bits, need %ld)\n", d, actual_bits, min_norm_bits);
      return false;
    }
  }
  return true;
}

/**
 * Evaluates the spectral test for a given multiplier lambda in higher dimensions (128-bit).
 *
 * @param lambda The multiplier to evaluate.
 * @return True if the spectral test passes, false otherwise.
 */
bool passes_higher_dimensions_128(uint128_t lambda) {
  NTL::ZZ a = (NTL::ZZ(get_hi128(lambda)) << 64) | NTL::ZZ(get_lo128(lambda));
  NTL::ZZ m = NTL::ZZ(1) << 128;

  std::print("\n[CPU 128] 2D Passed. Evaluating 0x%016llx%016llx in 3D-6D...\n", (unsigned long long)get_hi128(lambda), (unsigned long long)get_lo128(lambda));
  return evaluate_spectral_test(a, m, 128);
}

/**
 * Evaluates the spectral test for a given multiplier lambda in higher dimensions (64-bit).
 *
 * @param lambda The multiplier to evaluate.
 * @return True if the spectral test passes, false otherwise.
 */
bool passes_higher_dimensions_64(unsigned long long lambda) {
  NTL::ZZ a = NTL::ZZ(lambda);
  NTL::ZZ m = NTL::ZZ(1) << 64;

  std::print("\n[CPU 64] 2D Passed. Evaluating 0x%016llx in 3D-6D...\n", lambda);
  return evaluate_spectral_test(a, m, 64);
}
