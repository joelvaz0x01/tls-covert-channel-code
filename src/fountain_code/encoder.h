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
 * XORs the first k blocks together into the packet data. The caller
 * must place the selected source blocks at positions 0..k-1.
 *
 * @param pkt Pointer to the packet to fill.
 * @param id Packet ID.
 * @param k Number of source blocks to XOR.
 * @param blocks Pointer to the first k source blocks.
 */
void encode_packet(packet_t* pkt, const int id, const uint64_t k, const block_t* blocks);

/**
 * Feeds a packet to the decoder.
 *
 * @param dec Pointer to the decoder structure.
 * @param pkt Pointer to the packet to feed.
 * @return true if the packet added a new pivot (useful equation), false if it was linearly dependent (redundant).
 */
bool decoder_feed(decoder_t* dec, const packet_t* pkt);

#endif /* FOUNTAIN_CODE_ENCODER_H */
