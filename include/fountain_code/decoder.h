/**
 * @file decoder.h
 * @brief Header file for the decoder interface for Gaussian elimination over GF(2).
 */

#ifndef FOUNTAIN_CODE_DECODER_H
#define FOUNTAIN_CODE_DECODER_H

#include <fountain_code/fountain_code.h>

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
 */
typedef struct {
  int           n, n_words;
  int           remaining;
  int           pivot_present[MAX_BLOCKS];
  vec_t         pivot_sel  [MAX_BLOCKS];
  unsigned char pivot_data [MAX_BLOCKS][BLOCK_SIZE];
} decoder_t;

/**
 * Initializes the decoder.
 *
 * @param dec Pointer to the decoder structure to initialize.
 * @param n Number of source blocks.
 */
void decoder_init(decoder_t *dec, int n);

/**
 * Feeds a packet to the decoder.
 *
 * @param dec Pointer to the decoder structure.
 * @param pkt Pointer to the packet to feed.
 * @return 1 if the packet added a new pivot (useful equation), 0 if it was linearly dependent (redundant).
 */
int decoder_feed(decoder_t *dec, const packet_t *pkt);

/**
 * Decoder: solve the decoder by back-substitution.
 *
 * This function reduces each pivot row so that it has exactly one
 * bit set (at its own pivot index), then copies the decoded blocks out.
 *
 * Scans i from n-1 down to 0, reducing each pivot row as it goes.
 *
 * @param dec Pointer to the decoder structure.
 * @param out_blocks Pointer to the output buffer where decoded blocks will be copied.
 */
void decoder_solve(decoder_t *dec, unsigned char *out_blocks);

#endif /* FOUNTAIN_CODE_DECODER_H */
