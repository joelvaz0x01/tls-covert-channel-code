/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_SETTINGS_H
#define FOUNTAIN_CODE_SETTINGS_H

#include <stdint.h>

/** Hard upper limit on the number of source blocks. */
#ifndef MAX_BLOCKS
#define MAX_BLOCKS 1000
#endif

/** Size of one source block in bits. */
#define FC_BLOCK_SIZE 160

/** Words needed to store one source block. */
#define BLOCK_WORDS   ((FC_BLOCK_SIZE + 63) / 64)

/** Words needed to store a selector vector for MAX_BLOCKS. */
#define VEC_WORDS     ((MAX_BLOCKS + 63) / 64)

/* Constants for the Coupon Collector's problem mitigation. */
#define ALPHA         2.5
#define EULER         0.5772156649015329

#if MAX_BLOCKS > (UINT64_MAX - 63)
#error "MAX_BLOCKS must be at most 64 bits long"
#endif

#if FC_BLOCK_SIZE < 1
#error "FC_BLOCK_SIZE must be at least 1"
#endif

#if MAX_BLOCKS < 2
#error "MAX_BLOCKS must be at least 2"
#endif

#endif /* FOUNTAIN_CODE_SETTINGS_H */
