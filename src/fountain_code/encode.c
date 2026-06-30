/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Reference algorithm: Tomás Oliveira e Silva, October 2025
 */

#include <stdint.h>
#include <string.h>

#include "encoder.h"
#include "utils.h"
#include "vec_ops.h"

void encode_packet(packet_t* pkt, const int id, const uint64_t k, const block_t* blocks) {
  pkt->id = id;
  memset(&pkt->data, 0, sizeof(pkt->data));

  for (uint64_t i = 0; i < k; i++)
    data_xor(&pkt->data, &blocks[i]);
}
