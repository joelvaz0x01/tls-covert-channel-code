/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef CBPRNG_SETTINGS_H
#define CBPRNG_SETTINGS_H

#include <stdint.h>

#ifndef CBPRNG_BITS
#define CBPRNG_BITS 64
#endif

#ifndef S_BOX_BITS
#define S_BOX_BITS 4
#endif

#ifndef N_LAYERS
#define N_LAYERS 16
#endif

#if S_BOX_BITS < 3
#error "Bits per S-box must be at least 3"
#endif

#if CBPRNG_BITS % S_BOX_BITS != 0
#error "CBPRNG_BITS is not a multiple of S_BOX_BITS"
#endif

#if CBPRNG_BITS <= 256 && S_BOX_BITS <= 8
typedef uint8_t perm_val_t;
#elif CBPRNG_BITS <= 65536 && S_BOX_BITS <= 16
typedef uint16_t perm_val_t;
#else
typedef uint32_t perm_val_t;
#endif

#endif /* CBPRNG_SETTINGS_H */
