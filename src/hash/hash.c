/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "settings.h"

int hash_compute(const hash_algo_t* algo, const void* data, size_t bit_len, uint8_t* digest) {
  if (NULL == algo || NULL == digest) {
    return 0;
  }

  void* ctx = malloc(algo->context_size);
  if (NULL == ctx) {
    return 0;
  }

  if (!algo->init(ctx)) {
    free(ctx);
    return 0;
  }

  if (!algo->update(ctx, data, bit_len)) {
    free(ctx);
    return 0;
  }

  if (!algo->final(ctx, digest)) {
    free(ctx);
    return 0;
  }

  free(ctx);
  return 1;
}

int hash_compute_max_bits(const hash_algo_t* algo, const void* data, size_t bit_len, uint8_t* digest) {
  if (NULL == algo || NULL == digest) {
    return 0;
  }

  uint8_t* full_digest = (uint8_t*)malloc(algo->digest_size);
  if (NULL == full_digest) {
    return 0;
  }

  if (!hash_compute(algo, data, bit_len, full_digest)) {
    free(full_digest);
    return 0;
  }

  size_t bits_to_copy = HASH_OUTPUT_SIZE;
  size_t bytes_to_copy = bits_to_copy / 8;
  size_t remaining_bits = bits_to_copy % 8;

  memcpy(digest, full_digest, bytes_to_copy);

  if (remaining_bits > 0) {
    uint8_t mask = (0xFF << (8 - remaining_bits)) & 0xFF;
    digest[bytes_to_copy] = full_digest[bytes_to_copy] & mask;
  }

  free(full_digest);
  return 1;
}
