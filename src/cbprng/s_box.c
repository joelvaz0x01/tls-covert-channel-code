/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Reference algorithm: Tomás Oliveira e Silva, March 2026
 */

#include <stdbool.h>

#include "cbprng.h"
#include "s_box.h"
#include "settings.h"

/**
 * Checks if the S-box passes the completeness test.
 * This test ensures that every output bit depends on every input bit.
 *
 * @param n The size of the S-box (number of entries).
 * @param a The S-box permutation array.
 * @return true if it passes, false otherwise.
 */
static inline bool completeness_test(int n, const permutation_t* a) {
  for (int bit_mask_out = 1; bit_mask_out < n; bit_mask_out <<= 1) {
    for (int bit_mask_in = 1; bit_mask_in < n; bit_mask_in <<= 1) {
      int idx;
      for (idx = 0; idx < n && ((a[idx] & bit_mask_out) == 0) == ((a[idx ^ bit_mask_in] & bit_mask_out) == 0); idx++);
      if (idx == n) return false;
    }
  }
  return true;
}

/**
 * Checks if the S-box is nonlinear.
 * This test ensures that the S-box cannot be represented as a linear combination
 * of input bits using XOR.
 *
 * @param n The size of the S-box (number of entries).
 * @param a The S-box permutation array.
 * @return true if it passes, false otherwise.
 */
static inline bool nonlinearity_test(int n, const permutation_t* a) {
  for (int idx1 = 0; idx1 < n; idx1++) {
    int idx2;
    for (idx2 = 0; idx2 < n && (a[idx1] ^ a[idx2]) == a[idx1 ^ idx2]; idx2++);
    if (idx2 == n) return false;
  }
  return true;
}

void pseudo_random_s_box(s_box_t* s) {
  int n = 1 << S_BOX_BITS;
  do {
    pseudo_random_permutation(n, s->a);
  } while (!completeness_test(n, s->a) || !nonlinearity_test(n, s->a));
}
