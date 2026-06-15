/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_ENCODER_H
#define FOUNTAIN_CODE_ENCODER_H

#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

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
packet_t encode_packet(int id, const block_t* blocks, uint64_t n, uint64_t m, uint64_t n_words);

/**
 * Feeds a packet to the decoder.
 *
 * @param dec Pointer to the decoder structure.
 * @param pkt Pointer to the packet to feed.
 * @return true if the packet added a new pivot (useful equation), false if it was linearly dependent (redundant).
 */
bool decoder_feed(decoder_t* dec, const packet_t* pkt);

#endif /* FOUNTAIN_CODE_ENCODER_H */
