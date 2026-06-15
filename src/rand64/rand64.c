/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Adapted from the glibc random() and srandom() implementations
 * to operate on 64-bit state words instead of 32-bit.
 *
 * Uses the TYPE_3 trinomial x^31 + x^3 + 1 (degree 31, separation 3)
 * identical degree and separation to glibc 32-bit default, but adapted
 * to uint64_t state elements.
 */

#include <stdint.h>

#include "rand64.h"
#include "state.h"

uint64_t prng_state[RAND64_DEG];
uint64_t* prng_fptr = prng_state + PRNG_SEP;
uint64_t* prng_rptr = prng_state;
int prng_initialized = 0;
uint64_t prng_seed = 0;

/**
 * Computes the next value in LCG.
 *
 * @param x The current value in the LCG sequence.
 * @return The next value in the LCG sequence.
 */
static inline uint64_t lcg64(uint64_t x) {
  return INIT_MUL * x + INIT_ADD;
}

void srand64(uint64_t seed) {
  if (!seed) seed = 1; /* initialize seed to 1 to avoid seed == 0, matching the glibc srandom() convention. */

  prng_seed = seed;

  prng_state[0] = seed;
  for (int i = 1; i < RAND64_DEG; i++) /* fills the 31-element state table using Knuth's 64-bit LCG multiplier */
    prng_state[i] = lcg64(prng_state[i - 1]);

  /* initialize the generator state pointers */
  prng_fptr = prng_state + PRNG_SEP;
  prng_rptr = prng_state;

  /* remove linear correlations with DEG * 10 discard steps */
  for (int i = RAND64_DEG * 10; i-- > 0;) {
    uint64_t val = *prng_fptr + *prng_rptr;
    *prng_fptr = val;
    if (++prng_fptr >= prng_state + RAND64_DEG) prng_fptr = prng_state;
    if (++prng_rptr >= prng_state + RAND64_DEG) prng_rptr = prng_state;
  }

  prng_initialized = 1;
}

uint64_t rand64(void) {
  if (!prng_initialized) srand64(1); /* seed defaults to 1 if not initialized */

  uint64_t val = *prng_fptr + *prng_rptr;
  *prng_fptr = val;

  /* advance the generator state pointers */
  if (++prng_fptr >= prng_state + RAND64_DEG) prng_fptr = prng_state;
  if (++prng_rptr >= prng_state + RAND64_DEG) prng_rptr = prng_state;

  /* discard the least-significant bit (least-random), matching glibc random() behavior */
  return val >> 1;
}

uint64_t rand64_between(uint64_t min, uint64_t max) {
  return min + rand64() % (max - min + 1);
}
