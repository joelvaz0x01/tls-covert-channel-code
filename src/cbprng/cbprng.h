/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_H
#define CBPRNG_H

#include <stdint.h>

#include "p_box.h"
#include "s_box.h"
#include "settings.h"

typedef uint64_t mask_t;

/**
 * @struct generator_t
 * Represents the full pseudo-random generator.
 *
 * @var S The S-boxes for each layer.
 * @var P The P-boxes for each layer.
 */
typedef struct {
  s_box_t S[N_LAYERS][CBPRNG_BITS / S_BOX_BITS];
  p_box_t P[N_LAYERS - 1];
} generator_t;

/**
 * Fills an array with a pseudo-random permutation of integers from 0 to n-1.
 *
 * @param n The number of elements in the array.
 * @param a The array to fill with the permutation.
 */
void pseudo_random_permutation(int n, int a[n]);

/**
 * Initializes the full pseudo-random generator.
 *
 * @param g Pointer to the generator to initialize.
 */
void pseudo_random_generator(generator_t* g);

/**
 * Generates a pseudo-random number for a given counter value.
 *
 * @param g Pointer to the generator.
 * @param counter_value The counter value.
 * @param verbose If non-zero, prints internal state transitions.
 * @return The generated pseudo-random number.
 */
mask_t generate_cbprng(generator_t* g, mask_t counter_value);

#endif /* CBPRNG_H */
