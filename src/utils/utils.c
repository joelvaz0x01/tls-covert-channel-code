/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cbprng/cbprng.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include "settings.h"
#include "utils.h"

generator_t cbprng;
mask_t counter_value = 0;
decoder_t* dec = NULL;
block_t* buffer = NULL;
uint64_t* k_list = NULL;
packet_t* g_scratch_pkt = NULL;

uint64_t init_program(uint64_t n, uint64_t m) {
  dec = calloc(1, sizeof(decoder_t));
  if (NULL == dec) {
    fprintf(stderr, "[-] could not allocate decoder.\n");
    exit(EXIT_FAILURE);
  }

  if (0 != decoder_init(dec, n)) {
    fprintf(stderr, "[-] could not initialize decoder for n=%lu.\n", n);
    free(dec);
    dec = NULL;
    exit(EXIT_FAILURE);
  }

  buffer = calloc((size_t)m, sizeof(block_t));
  if (NULL == buffer) {
    free(dec);

    dec = NULL;

    fprintf(stderr, "[-] could not allocate output buffer.\n");
    exit(EXIT_FAILURE);
  }

  k_list = malloc(m * sizeof(uint64_t));
  if (NULL == k_list) {
    free(dec);
    free(buffer);

    dec = NULL;
    buffer = NULL;

    fprintf(stderr, "[-] could not allocate k_list.\n");
    exit(EXIT_FAILURE);
  }

  g_scratch_pkt = calloc(1, sizeof(packet_t));
  if (NULL == g_scratch_pkt) {
    free(dec);
    free(buffer);
    free(k_list);

    dec = NULL;
    buffer = NULL;
    k_list = NULL;

    fprintf(stderr, "[-] could not allocate scratch packet.\n");
    exit(EXIT_FAILURE);
  }

  return ((n + 63) / 64);
}

void finalize_program(void) {
  decoder_destroy(dec);

  free(dec);
  free(buffer);
  free(k_list);
  free(g_scratch_pkt);

  dec = NULL;
  buffer = NULL;
  k_list = NULL;
  g_scratch_pkt = NULL;
}

void cypher_fountain(block_t* fc, const uint64_t seed, const uint64_t file_id) {
  sha256_ctx_t ctx;
  uint8_t digest[HASH_ALGORITHM->digest_size];

  HASH_ALGORITHM->init(&ctx);

  HASH_ALGORITHM->update(&ctx, &seed, CBPRNG_LEN);             /* seed                */
  HASH_ALGORITHM->update(&ctx, &file_id, sizeof(file_id) * 8); /* file identifier (i) */
  HASH_ALGORITHM->update(&ctx, KEY, KEY_LEN);                  /* constant key        */

  HASH_ALGORITHM->final(&ctx, digest);

  for (int i = 0; i < FC_LEN / 8; i++) {
    ((uint8_t*)fc)[i] ^= digest[i];
  }
}

void build_hash(const block_t* enc_b, uint64_t seed, const uint64_t file_id, const char* is_invalid, uint32_t* digest_out) {
  sha256_ctx_t ctx;
  uint8_t full_digest[HASH_ALGORITHM->digest_size];

  HASH_ALGORITHM->init(&ctx);

  HASH_ALGORITHM->update(&ctx, is_invalid, 8);                 /* valid data              */
  HASH_ALGORITHM->update(&ctx, enc_b, FC_LEN);                 /* encrypted fountain code */
  HASH_ALGORITHM->update(&ctx, &seed, CBPRNG_LEN);             /* seed                    */
  HASH_ALGORITHM->update(&ctx, &file_id, sizeof(file_id) * 8); /* file identifier (i)     */
  HASH_ALGORITHM->update(&ctx, KEY, KEY_LEN);                  /* constant key            */

  HASH_ALGORITHM->final(&ctx, full_digest);

  memcpy(digest_out, full_digest, 4);
}

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

void write_fountain(const tls_mod_rand_t data, FILE* out) {
  fwrite(&data.cbprng, sizeof(mask_t), 1, out);
  for (int i = 0; i < BLOCK_WORDS - 1; i++) {
    fwrite(&data.fountain_code.w[i], sizeof(uint64_t), 1, out);
  }
  fwrite(&data.fountain_code.w[BLOCK_WORDS - 1], sizeof(uint32_t), 1, out);
  fwrite(&data.hash, sizeof(uint32_t), 1, out);
}
