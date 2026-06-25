/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>

#include <cbprng/cbprng.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include "encoder.h"
#include "utils.h"

generator_t cbprng;
mask_t counter_value = 0;

void build_fountain(packet_t* pkt, const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words) {
  for (;;) {
    uint64_t k = generate_k(m, *seed, n, k_list);
    vec_zero(dec->scratch_sel, n_words);
    for (uint64_t i = 0; i < k; i++)
      vec_set(dec->scratch_sel, k_list[i]);
    encode_packet(pkt, id, k, buffer);

    if (decoder_feed(dec, pkt)) return;

    counter_value++;
    *seed = generate_cbprng(&cbprng, counter_value);
  }
}

tls_mod_rand_t modified_random_field(const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words) {
  build_fountain(g_scratch_pkt, id, seed, m, n, n_words);

  tls_mod_rand_t result;
  result.cbprng = *seed;
  result.fountain_code = g_scratch_pkt->data;

  cypher_fountain(&result.fountain_code, *seed, id);
  build_hash(&result.fountain_code, *seed, id, "0", &result.hash);

  return result;
}
