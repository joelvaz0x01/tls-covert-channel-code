/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Reference algorithm: Tomás Oliveira e Silva, October 2025
 */

#include <stdint.h>
#include <string.h>

#include "decoder.h"
#include "utils.h"
#include "vec_ops.h"

void decoder_solve(decoder_t* dec, block_t* out_blocks) {
  uint64_t n = dec->n;
  uint64_t nw = dec->n_words;

  for (uint64_t i = n; i-- > 0;) {
    if (!dec->pivot_present[i]) continue;
    for (uint64_t j = i + 1; j < n; j++) {
      if (!dec->pivot_present[j]) continue;
      if (!vec_test(&dec->pivot_sel[i], j)) continue;
      vec_xor(&dec->pivot_sel[i], &dec->pivot_sel[j], nw);
      data_xor(&dec->pivot_data[i], &dec->pivot_data[j]);
    }
  }

  for (uint64_t i = 0; i < n; i++) {
    if (dec->pivot_present[i]) {
      out_blocks[i] = dec->pivot_data[i];
    } else {
      memset(&out_blocks[i], 0, sizeof(block_t));
    }
  }
}
