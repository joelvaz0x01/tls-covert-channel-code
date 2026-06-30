/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Reference algorithm: Tomás Oliveira e Silva, March 2026
 */

#ifndef CBPRNG_H
#define CBPRNG_H

#include <stdbool.h>
#include <stdint.h>

#include "p_box.h"
#include "s_box.h"
#include "settings.h"

/**
 * @brief Bit mask type matching the generator width (up to 64 bits).
 */
typedef uint64_t mask_t;

/**
 * @struct generator_t
 * Full counter-based pseudo-random number generator state.
 *
 * Composed of alternating substitution (S-box) and permutation (P-box)
 * layers that together provide strong non-linearity and diffusion.
 *
 * @var S S-boxes for each layer [N_LAYERS][CBPRNG_BITS / S_BOX_BITS].
 * @var P P-boxes for each consecutive layer pair [N_LAYERS - 1].
 */
typedef struct {
  s_box_t S[N_LAYERS][CBPRNG_BITS / S_BOX_BITS];
  p_box_t P[N_LAYERS - 1];
} generator_t;

/**
 * Fills an array with a uniformly random permutation of [0, n-1].
 *
 * @param n Number of elements.
 * @param a Output array receiving the permutation.
 */
void pseudo_random_permutation(int n, permutation_t* a);

/**
 * Initialises the full CBPRNG generator.
 *
 * @param g Pointer to the generator to initialise.
 */
void pseudo_random_generator(generator_t* g);

/**
 * Evaluates the CBPRNG with configurable parameters.
 *
 * @param S Flat array of all S-box permutations (layer-major).
 * @param P Flat array of all P-box permutations (layer-major).
 * @param counter_value Input counter value.
 * @param bits Width of the generator in bits.
 * @param layers Number of S-box layers.
 * @param sbox_bits Number of input/output bits per S-box.
 * @return The generated pseudo-random number.
 */
mask_t generate_cbprng_generic(permutation_t* S, permutation_t* P, mask_t counter_value, int bits, int layers, int sbox_bits);

/**
 * Evaluates the default CBPRNG for a given counter value.
 *
 * @param g Pointer to an initialized generator.
 * @param counter_value Input counter value.
 * @return The generated pseudo-random number.
 */
mask_t generate_cbprng(generator_t* g, mask_t counter_value);

#endif /* CBPRNG_H */
