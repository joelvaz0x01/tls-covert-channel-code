/**
 * @file encoder.h
 * @brief Header file for the encoder interface for fountain code.
 */

#ifndef FOUNTAIN_CODE_ENCODER_H
#define FOUNTAIN_CODE_ENCODER_H

#include <fountain_code/fountain_code.h>

/**
 * Encoder: produce one fountain-code packet.
 *
 * Selects m source-block indices with replacement; vec_set is
 * idempotent so duplicate picks are silently merged (matching the
 * reference algorithm).  Data = XOR of every block whose bit is set.
 *
 * @param id Packet ID.
 * @param blocks Pointer to the source blocks.
 * @param n Number of source blocks.
 * @param m Number of blocks to select.
 * @param nw Number of words in the selector vector.
 * @return The encoded packet.
 */
packet_t encode_packet(int id, const unsigned char *blocks, int n, int m, int n_words);

#endif /* FOUNTAIN_CODE_ENCODER_H */
