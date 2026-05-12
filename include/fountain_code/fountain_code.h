/**
 * @file fountain_code.h
 * @brief Header file for the fountain code file-transfer library.
 */

#ifndef FOUNTAIN_CODE_H
#define FOUNTAIN_CODE_H

#define BLOCK_SIZE  20                      /**< source-block size (bytes)      */
#define MAX_BLOCKS  1000                    /**< hard upper limit on n          */
#define EULER       0.5772156649015329      /**< Euler–Mascheroni constant      */
#define VEC_WORDS  ((MAX_BLOCKS + 63) / 64) /**< number of words in the vector  */

/**
 * @typedef u64_t
 * Represents a 64-bit unsigned integer.
 */
typedef unsigned long u64_t;

/**
 * @struct vec_t
 * Represents a vector used in Gaussian elimination over GF(2).
 *
 * @var w Array of word-sized selector vectors.
 */
typedef struct { u64_t w[VEC_WORDS]; } vec_t;

/**
 * @struct packet_t
 * Represents a fountain-code packet.
 *
 * @var id Packet ID.
 * @var selector Selector vector indicating which source blocks are included.
 * @var data Packet data (XOR of selected source blocks).
 */
typedef struct {
  int           id;
  vec_t         selector;
  unsigned char data[BLOCK_SIZE];
} packet_t;

/**
 * Prints the given selector as a binary string of exactly n characters.
 *
 * @param v Pointer to the selector to print.
 * @param n Number of characters to print.
 */
void print_sel(const vec_t *v, int n);

#endif /* FOUNTAIN_CODE_H */
