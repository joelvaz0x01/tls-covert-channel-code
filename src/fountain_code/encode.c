/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Reference algorithm: Tomás Oliveira e Silva, October 2025
 */

#include <stdbool.h>
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

bool decoder_feed(decoder_t* dec, const packet_t* pkt) {
  vec_t* sel = dec->scratch_sel;
  block_t dat = pkt->data;

  for (;;) {
    int64_t i = vec_lsb(sel, dec->n_words);
    if (i < 0) return false;

    if (!dec->pivot_present[i]) {
      dec->pivot_present[i] = true;
      for (uint64_t j = 0; j < dec->n_words; j++)
        dec->pivot_sel[i].w[j] = sel->w[j];
      dec->pivot_data[i] = dat;
      dec->remaining--;
      return true;
    }
    for (uint64_t j = 0; j < dec->n_words; j++)
      dec->workspace->w[j] = dec->pivot_sel[i].w[j];
    vec_xor(sel, dec->workspace, dec->n_words);
    data_xor(&dat, &dec->pivot_data[i]);
  }
}
