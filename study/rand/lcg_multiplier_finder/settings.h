/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef LCG_MULT_FINDER_SETTINGS_H
#define LCG_MULT_FINDER_SETTINGS_H

/* GPU search limits */
#define MAX_K_128                5
#define MAX_K_64                 3
#define MAX_CANDIDATES_PER_BATCH 100

/* default starting point: Golden Ratio fractional bits (64-bit) */
#define GOLDEN_RATIO_64          0x9e3779b97f4a7c15ULL

/* default starting point: Golden Ratio fractional bits (128-bit) */
#define GOLDEN_RATIO_128_HI      GOLDEN_RATIO_64
#define GOLDEN_RATIO_128_LO      0xf39cc0605cedc835ULL

/* checkpoint save interval (batches) */
#define CHECKPOINT_INTERVAL      1000

#endif /* LCG_MULT_FINDER_SETTINGS_H */
