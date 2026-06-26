/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_UTILS_H
#define FOUNTAIN_CODE_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#include "settings.h"

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
 * @var data Packet data (XOR of selected source blocks).
 */
typedef struct {
  int id;
  block_t data;
} packet_t;

/**
 * @struct decoder_t
 * Represents the decoder state for Gaussian elimination over GF(2).
 *
 * @var n Number of source blocks.
 * @var n_words Number of word-sized selector vectors.
 * @var remaining Number of blocks still needed.
 * @var pivot_present Whether each block has a pivot.
 * @var pivot_sel Pivot selector for each block.
 * @var pivot_data Pivot data for each block.
 * @var scratch_sel Selector for the incoming packet.
 */
typedef struct {
  uint64_t n;
  uint64_t n_words;
  uint64_t remaining;
  bool* pivot_present;
  vec_t* pivot_sel;
  block_t* pivot_data;
  vec_t* scratch_sel;
  vec_t* workspace;
} decoder_t;

/**
 * Initializes the decoder.
 *
 * @param dec Pointer to the decoder structure to initialize.
 * @param n Number of source blocks.
 * @return 0 on success, -1 on allocation failure.
 */
int decoder_init(decoder_t* dec, const uint64_t n);

/**
 * Destroys the decoder, freeing internal allocations.
 *
 * @param dec Pointer to the decoder structure.
 */
void decoder_destroy(decoder_t* dec);

/**
 * Calculates the number of parts to encode on Fountain Code.
 *
 * @param n The number of blocks.
 * @return The number of parts to generate.
 */
uint64_t generate_m(const uint64_t n);

/**
 * Generates m unique values in [0, n-1] using a seed and stores them
 * in the out array. Duplicates from the random draws will be discarded.
 *
 * @param m The number of values to draw.
 * @param seed The seed for the pseudo-random number generator.
 * @param n Upper bound (exclusive) for the generated values.
 * @param out Pointer to the array where generated values will be stored.
 * @return The number of unique values stored in out (may be less than m).
 */
uint64_t generate_k(const uint64_t m, const uint64_t seed, const uint64_t n, uint64_t* out);

#endif /* FOUNTAIN_CODE_UTILS_H */
