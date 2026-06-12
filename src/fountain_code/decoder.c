/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <string.h>

#include "decoder.h"
#include "utils.h"

void decoder_solve(decoder_t* dec, block_t* out_blocks) {
  int n = dec->n;
  int nw = dec->n_words;

  for (int i = n - 1; i >= 0; i--) {
    if (!dec->pivot_present[i]) continue;
    for (int j = i + 1; j < n; j++) {
      if (!dec->pivot_present[j]) continue;
      if (!vec_test(&dec->pivot_sel[i], j)) continue;
      vec_xor(&dec->pivot_sel[i], &dec->pivot_sel[j], nw);
      data_xor(&dec->pivot_data[i], &dec->pivot_data[j]);
    }
  }

  for (int i = 0; i < n; i++) {
    if (dec->pivot_present[i]) {
      out_blocks[i] = dec->pivot_data[i];
    } else {
      memset(&out_blocks[i], 0, sizeof(block_t));
    }
  }
}
