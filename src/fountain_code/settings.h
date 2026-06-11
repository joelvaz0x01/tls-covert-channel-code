/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_SETTINGS_H
#define FOUNTAIN_CODE_SETTINGS_H

#include <stdint.h>

#define FC_BLOCK_SIZE 160                         /**< source-block size in bits           */
#define BLOCK_WORDS   ((FC_BLOCK_SIZE + 63) / 64) /**< number of words in the source-block */
#define MAX_BLOCKS    1000                        /**< hard upper limit on n               */
#define EULER         0.5772156649015329          /**< Euler–Mascheroni constant           */
#define VEC_WORDS     ((MAX_BLOCKS + 63) / 64)    /**< number of words in the vector       */

/**
 * @struct vec_t
 * Represents a vector used in Gaussian elimination over GF(2).
 *
 * @var w Array of word-sized selector vectors.
 */
typedef struct {
  uint64_t w[VEC_WORDS];
} vec_t;

/**
 * @struct block_t
 * Represents a source block or encoded data block.
 *
 * @var w Array of word-sized data.
 */
typedef struct {
  uint64_t w[BLOCK_WORDS];
} block_t;

/**
 * @struct packet_t
 * Represents a fountain-code packet.
 *
 * @var id Packet ID.
 * @var selector Selector vector indicating which source blocks are included.
 * @var data Packet data (XOR of selected source blocks).
 */
typedef struct {
  int id;
  vec_t selector;
  block_t data;
} packet_t;

#endif /* FOUNTAIN_CODE_SETTINGS_H */
