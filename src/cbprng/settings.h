/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_SETTINGS_H
#define CBPRNG_SETTINGS_H

#include <stdint.h>

/** Width of the CBPRNG in bits. */
#ifndef CBPRNG_BITS
#define CBPRNG_BITS 64
#endif

/** Number of input/output bits per S-box. */
#ifndef S_BOX_BITS
#define S_BOX_BITS 4
#endif

/** Number of S-box layers in the generator. */
#ifndef N_LAYERS
#define N_LAYERS 16
#endif

#if S_BOX_BITS < 3
#error "S_BOX_BITS must be at least 3"
#endif

#if CBPRNG_BITS > 64
#error "CBPRNG_BITS must be at most 64"
#endif

#if CBPRNG_BITS % S_BOX_BITS != 0
#error "CBPRNG_BITS must be a multiple of S_BOX_BITS"
#endif

/* Use the smallest unsigned type that can hold a permutation index. */
#if S_BOX_BITS <= 8
typedef uint8_t permutation_t;
#elif S_BOX_BITS <= 16
typedef uint16_t permutation_t;
#elif S_BOX_BITS <= 32
typedef uint32_t permutation_t;
#else
typedef uint64_t permutation_t;
#endif

#endif /* CBPRNG_SETTINGS_H */
