/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_ENCODER_H
#define FOUNTAIN_CODE_ENCODER_H

#include "settings.h"

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
packet_t encode_packet(int id, const block_t* blocks, int n, int m, int n_words);

#endif /* FOUNTAIN_CODE_ENCODER_H */
