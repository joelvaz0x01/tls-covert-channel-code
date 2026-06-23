/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef HASH_SHA256_H
#define HASH_SHA256_H

#include "hash.h"

extern const hash_algo_t sha256;

#define SHA256 (&sha256)

/**
 * @struct sha256_ctx_t
 * The SHA-256 context structure.
 *
 * @var data The data buffer for the SHA-256 algorithm.
 * @var bitlen_buffer The number of bits currently in the data buffer (0-511).
 * @var bitlen_total The total number of bits processed.
 * @var state The SHA-256 state.
 */
typedef struct {
  uint8_t data[64];
  uint32_t bitlen_buffer;
  uint64_t bitlen_total;
  uint32_t state[8];
} sha256_ctx_t;

/**
 * Initializes the SHA-256 context.
 *
 * @param ctx The SHA-256 context to initialize.
 */
int sha256_init(void* ctx);

/**
 * Updates the SHA-256 context with new data.
 *
 * @param ctx The SHA-256 context.
 * @param data The data to process.
 * @param bit_len The length of the data in BITS.
 */
int sha256_update(void* ctx, const void* data, size_t bit_len);

/**
 * Finalizes the SHA-256 hash and produces the digest.
 *
 * @param ctx The SHA-256 context.
 * @param digest The buffer to store the 32-byte hash result.
 */
int sha256_final(void* ctx, uint8_t* digest);

#endif /* HASH_SHA256_H */
