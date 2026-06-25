/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND_SETTINGS_H
#define RAND_SETTINGS_H

#include <stdint.h>

#include <uint128/uint128.h>

/*
 * 128-bit LCG values; the multiplier derived via
 * Borosh-Niederreiter methodology where K <= 5.
 *
 * LCG multiplier is split into two 64-bit halves:
 *   - value: 210306068529402873165736369884852939165
 */
#define LCG_MUL_HI UINT64_C(0x9E3779B97F4A7C15)
#define LCG_MUL_LO UINT64_C(0xF39CC0608F08699D)

#define LCG_MUL    U128(LCG_MUL_LO, LCG_MUL_HI)
#define LCG_ADD    U128(1, 0)

#endif /* RAND_SETTINGS_H */
