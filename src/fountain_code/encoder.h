/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_ENCODER_H
#define FOUNTAIN_CODE_ENCODER_H

#include <stdint.h>

#include "utils.h"

/**
 * Produces one fountain-code packet by XORing k source blocks.
 *
 * The caller must place the selected source blocks at positions 0..k-1.
 *
 * @param pkt Pointer to the packet to fill.
 * @param id Packet ID.
 * @param k Number of source blocks to XOR together.
 * @param blocks Pointer to the first k source blocks.
 */
void encode_packet(packet_t* pkt, const int id, const uint64_t k, const block_t* blocks);

#endif /* FOUNTAIN_CODE_ENCODER_H */
