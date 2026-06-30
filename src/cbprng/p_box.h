/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_P_BOX_H
#define CBPRNG_P_BOX_H

#include "settings.h"

/**
 * Initializes a pseudo-random permutation for a P-box.
 *
 * @param n Number of elements in the permutation.
 * @param a Output array receiving the permutation.
 */
void pseudo_random_permutation(int n, permutation_t* a);

/**
 * @struct p_box_t
 * Represents a pseudo-random P-box (permutation box).
 *
 * @var a Permutation array: output position for each of the CBPRNG_BITS inputs.
 */
typedef struct {
  permutation_t a[CBPRNG_BITS];
} p_box_t;

/**
 * Initializes a pseudo-random P-box.
 *
 * @param p Pointer to the P-box to initialize.
 */
static inline void pseudo_random_p_box(p_box_t* p) {
  pseudo_random_permutation(CBPRNG_BITS, p->a);
}

#endif /* CBPRNG_P_BOX_H */
