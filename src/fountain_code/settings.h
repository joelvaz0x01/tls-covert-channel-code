/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_SETTINGS_H
#define FOUNTAIN_CODE_SETTINGS_H

#define FC_BLOCK_SIZE 160                         /**< source-block size in bits           */
#define BLOCK_WORDS   ((FC_BLOCK_SIZE + 63) / 64) /**< number of words in the source-block */
#define MAX_BLOCKS    1000                        /**< hard upper limit on n               */
#define EULER         0.5772156649015329          /**< Euler–Mascheroni constant           */
#define VEC_WORDS     ((MAX_BLOCKS + 63) / 64)    /**< number of words in the vector       */

#endif /* FOUNTAIN_CODE_SETTINGS_H */
