/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_P_BOX_H
#define CBPRNG_P_BOX_H

#include "settings.h"

/**
 * @struct p_box_t
 * Represents a pseudo-random P-box.
 *
 * @var a The permutation array.
 */
typedef struct {
  int a[CBPRNG_BITS];  // bit idx goes to bit a[idx]
} p_box_t;

/**
 * Initializes a pseudo-random P-box.
 *
 * @param p Pointer to the P-box to initialize.
 */
void pseudo_random_p_box(p_box_t* p);

#endif /* CBPRNG_P_BOX_H */
