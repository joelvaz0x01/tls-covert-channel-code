/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include "file.h"
#include "settings.h"
#include "utils.h"

decoder_t* dec = NULL;
block_t* buffer = NULL;
uint64_t* k_list = NULL;
packet_t* g_scratch_pkt = NULL;

uint64_t calculate_n(const char* filename, const bool is_encoder) {
  if (0 == open_file(filename, 1)) return 0;

  if (is_encoder)
    return ((uint64_t)get_src_size() + FC_LEN_BYTES - 1) / FC_LEN_BYTES;

  return (uint64_t)get_src_size() / RANDOM_FIELD_LEN_BYTES;
}

uint64_t init_program(uint64_t n, uint64_t m) {
  dec = calloc(1, sizeof(decoder_t));
  if (NULL == dec) {
    fprintf(stderr, "[-] could not allocate decoder.\n");
    close_files();
    exit(EXIT_FAILURE);
  }

  if (0 != decoder_init(dec, n)) {
    fprintf(stderr, "[-] could not initialize decoder for n=%lu.\n", n);
    close_files();
    finalize_program();
    exit(EXIT_FAILURE);
  }

  buffer = calloc((size_t)m, sizeof(block_t));
  if (NULL == buffer) {
    fprintf(stderr, "[-] could not allocate output buffer.\n");
    close_files();
    finalize_program();
    exit(EXIT_FAILURE);
  }

  k_list = malloc(m * sizeof(uint64_t));
  if (NULL == k_list) {
    fprintf(stderr, "[-] could not allocate k_list.\n");
    close_files();
    finalize_program();
    exit(EXIT_FAILURE);
  }

  g_scratch_pkt = calloc(1, sizeof(packet_t));
  if (NULL == g_scratch_pkt) {
    fprintf(stderr, "[-] could not allocate scratch packet.\n");
    close_files();
    finalize_program();
    exit(EXIT_FAILURE);
  }

  return ((n + 63) / 64);
}

uint64_t reset_program(uint64_t n) {
  decoder_destroy(dec);

  if (0 != decoder_init(dec, n)) {
    fprintf(stderr, "[-] could not re-initialize decoder for n=%lu.\n", n);
    close_files();
    finalize_program();
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

uint64_t construct_k(const uint64_t seed, const uint64_t m, const uint64_t n, const uint64_t n_words, const char* src_file) {
  uint64_t k = generate_k(m, seed, n, k_list);
  vec_zero(dec->scratch_sel, n_words);
  for (uint64_t i = 0; i < k; i++) {
    vec_set(dec->scratch_sel, k_list[i]);

    if (NULL != src_file)
      if (-1 == read_file_part(src_file, k_list[i], &buffer[i]))
        return 0;
  }

  return k;
}

void cypher_fountain(block_t* fc, const uint64_t seed, const uint64_t file_id) {
  sha256_ctx_t ctx;
  uint8_t digest[HASH_ALGORITHM->digest_size];

  HASH_ALGORITHM->init(&ctx);

  HASH_ALGORITHM->update(&ctx, &seed, CBPRNG_LEN);             /* seed                */
  HASH_ALGORITHM->update(&ctx, &file_id, sizeof(file_id) * 8); /* file identifier (i) */
  HASH_ALGORITHM->update(&ctx, KEY, KEY_LEN);                  /* constant key        */

  HASH_ALGORITHM->final(&ctx, digest);

  for (int i = 0; i < FC_LEN / 8; i++)
    ((uint8_t*)fc)[i] ^= digest[i];
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
