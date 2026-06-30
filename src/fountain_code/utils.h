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
 * Allocates and initializes the decoder state.
 *
 * @param dec Pointer to the decoder structure to initialize.
 * @param n Number of source blocks.
 * @return 0 on success, -1 on allocation failure.
 */
int decoder_init(decoder_t* dec, const uint64_t n);

/**
 * Frees all memory owned by the decoder.
 *
 * @param dec Pointer to the decoder structure.
 */
void decoder_destroy(decoder_t* dec);

/**
 * Feeds one fountain-code packet into the Gaussian-elimination solver.
 *
 * Searches for a new pivot in the packet's selector vector. If a
 * linearly-independent pivot is found it is stored and true is returned;
 * if the equation is redundant (linearly dependent) false is returned.
 *
 * @param dec Pointer to the decoder state.
 * @param pkt Pointer to the incoming packet.
 * @return true if a new pivot was added, false if the packet was redundant.
 */
bool decoder_feed(decoder_t* dec, const packet_t* pkt);

/**
 * Calculates the number of blocks to XOR per encoded packet.
 *
 * @param n Number of source blocks.
 * @return Number of blocks to XOR per packet (always >= 1, < n).
 */
uint64_t generate_m(const uint64_t n);

/**
 * Draws m unique random indices in [0, n-1] using a given seed.
 * Duplicates from the random draws are discarded. The results are
 * sorted ascending before return.
 *
 * @param m Desired number of values to draw.
 * @param seed Seed for the PRNG.
 * @param n Upper bound (exclusive) for the generated values.
 * @param out Buffer of at least m elements for the result.
 * @return The number of unique values written to out (may be < m).
 */
uint64_t generate_k(const uint64_t m, const uint64_t seed, const uint64_t n, uint64_t* out);

#endif /* FOUNTAIN_CODE_UTILS_H */
