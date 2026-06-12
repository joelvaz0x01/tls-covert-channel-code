/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdlib.h>

#include "cbprng.h"

static int avalanche_analysis(generator_t* g) {
  mask_t all_bits_mask = (CBPRNG_BITS == 64) ? ~(mask_t)0 : ((mask_t)1 << CBPRNG_BITS) - 1;

  /* least significant S_BOX_BITS set to one */
  mask_t s_box_mask = (1ul << S_BOX_BITS) - 1u;
  for (int bit = 0; bit < CBPRNG_BITS; bit++) {
    mask_t affected_bits = 1ul << bit;
    for (int layer = 0; layer < N_LAYERS; layer++) {
      mask_t new_affected_bits = 0ul;

      /* test S-boxes layer */
      for (int s_box_idx = 0; s_box_idx < CBPRNG_BITS / S_BOX_BITS; s_box_idx++)
        /* if any input bit of the S_box is already affected, affect all output bits */
        if (0ul != ((affected_bits >> (s_box_idx * S_BOX_BITS)) & s_box_mask))
          new_affected_bits |= s_box_mask << (s_box_idx * S_BOX_BITS);

      /* test P-box layer */
      if (layer < N_LAYERS - 1) {
        affected_bits = 0ul;
        for (int idx = 0; idx < CBPRNG_BITS; idx++) /* map bit idx to bit a[idx] */
          affected_bits |= ((new_affected_bits >> idx) & 1ul) << g->P[layer].a[idx];
      } else
        affected_bits = new_affected_bits;
    }
    if (affected_bits != all_bits_mask) return 0;
  }
  return 1;
}

void pseudo_random_permutation(int n, int a[n]) {
  for (int idx = 0; idx < n; idx++) a[idx] = idx;
  for (int idx = n - 1; idx > 0; idx--) { /* 0 <= swap_idx <= idx */
    int swap_idx = (int)((unsigned int)rand() % (unsigned int)(idx + 1));
    int swap_data = a[swap_idx];
    a[swap_idx] = a[idx];
    a[idx] = swap_data;
  }
}

void pseudo_random_generator(generator_t* g) {
  do {
    for (int layer = 0; layer < N_LAYERS - 1; layer++)
      pseudo_random_p_box(&g->P[layer]);
    for (int layer = 0; layer < N_LAYERS; layer++)
      for (int s_box_idx = 0; s_box_idx < CBPRNG_BITS / S_BOX_BITS; s_box_idx++)
        pseudo_random_s_box(&g->S[layer][s_box_idx]);
  } while (!avalanche_analysis(g));
}

mask_t generate_cbprng_generic(int* S, int* P, mask_t counter_value, int bits, int layers, int sbox_bits) {
  mask_t s_box_mask = (1ul << sbox_bits) - 1u;
  mask_t state = counter_value;
  int n_sboxes = bits / sbox_bits;
  int sbox_size = 1 << sbox_bits;

  for (int layer = 0; layer < layers; layer++) {
    mask_t new_bits = 0ul;
    for (int s_box_idx = 0; s_box_idx < n_sboxes; s_box_idx++) {
      int idx = (int)((state >> (s_box_idx * sbox_bits)) & s_box_mask);
      int output = S[layer * n_sboxes * sbox_size + s_box_idx * sbox_size + idx];
      new_bits |= (mask_t)output << (s_box_idx * sbox_bits);
    }

    if (layer < layers - 1) {
      state = 0ul;
      for (int idx = 0; idx < bits; idx++)
        state |= ((new_bits >> idx) & 1ul) << P[layer * bits + idx];
    } else {
      state = new_bits;
    }
  }
  return state;
}

mask_t generate_cbprng(generator_t* g, mask_t counter_value) {
  return generate_cbprng_generic((int*)g->S, (int*)g->P, counter_value, CBPRNG_BITS, N_LAYERS, S_BOX_BITS);
}
