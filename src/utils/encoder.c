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

/**
 * Builds a fountain code packet.
 *
 * @param id The packet ID.
 * @param seed The CBPRNG based seed.
 * @param m The number of rows in the fountain code.
 * @param n The number of columns in the fountain code.
 * @param n_words The number of words in the packet.
 */
static void build_fountain(const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words, const char* src_file) {
  for (;;) {
    uint64_t k = construct_k(*seed, m, n, n_words, src_file);
    encode_packet(g_scratch_pkt, id, k, buffer);

    if (decoder_feed(dec, g_scratch_pkt)) return;

    counter_value++;
    *seed = generate_cbprng(&cbprng, counter_value);
  }
}

tls_mod_rand_t modified_random_field(const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words, const char* src_file) {
  build_fountain(id, seed, m, n, n_words, src_file);

  tls_mod_rand_t result;
  result.cbprng = *seed;
  result.enc_fc = g_scratch_pkt->data;

  cypher_fountain(&result.enc_fc, *seed, id);
  build_hash(&result.enc_fc, *seed, id, "0", &result.hash);

  return result;
}
