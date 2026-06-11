/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_S_BOX_H
#define CBPRNG_S_BOX_H

#include "settings.h"

/**
 * @struct s_box_t
 * Represents a pseudo-random S-box.
 *
 * @var a The permutation array.
 */
typedef struct {
  int a[1 << S_BOX_BITS];  // input idx gives the output a[idx]
} s_box_t;

/**
 * Initializes a pseudo-random S-box.
 *
 * @param s Pointer to the S-box to initialize.
 */
void pseudo_random_s_box(s_box_t* s);

#endif /* CBPRNG_S_BOX_H */
