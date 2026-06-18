/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_TESTU01_BIGCRUSH_UTILS_H
#define RAND_TESTU01_BIGCRUSH_UTILS_H

#include <stdint.h>

static int toggle = 0;
static uint64_t current_block;
static uint64_t (*rand_fn)(void);

/**
 * Calculates the next value from the LCG
 * and returns the upper or lower 32 bits alternatively.
 *
 * @return The upper or lower 32 bits alternatively of the current block.
 */
static inline unsigned int lcg_next(void) {
  if (toggle == 0) {
    current_block = rand_fn();
    toggle = 1;
    return (unsigned int)(current_block >> 32);
  } else {
    toggle = 0;
    return (unsigned int)(current_block & 0xFFFFFFFF);
  }
}

#endif /* RAND_TESTU01_BIGCRUSH_UTILS_H */
