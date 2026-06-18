/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_UINT128_T_H
#define LCG_MULT_FINDER_UINT128_T_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct uint128_t
 * Represents a 128-bit unsigned integer using two 64-bit unsigned integers.
 *
 * @var lo The lower 64 bits of the 128-bit integer.
 * @var hi The higher 64 bits of the 128-bit integer.
 */
typedef struct {
  unsigned long long lo;
  unsigned long long hi;
} uint128_t;

#ifdef __cplusplus
}
#endif

#endif /* LCG_MULT_FINDER_UINT128_T_H */
