/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define FN_ static inline __attribute__((const))
#elif defined(_MSC_VER)
#define FN_ static __forceinline
#else
#define FN_ static inline
#endif

/**
 * @struct hash_algo
 * Interface for a hash algorithm.
 *
 * @var name The name of the hash algorithm.
 * @var digest_size The size of the hash digest in bytes.
 * @var context_size The size of the hash context in bytes.
 * @var init Function pointer to initialize the hash context.
 * @var update Function pointer to update the hash context with data.
 * @var final Function pointer to finalize the hash and produce the digest.
 */
typedef struct hash_algo {
  const char* name;
  size_t digest_size;
  size_t context_size;
  int (*init)(void* ctx);
  int (*update)(void* ctx, const void* data, size_t bit_len);
  int (*final)(void* ctx, uint8_t* digest);
} hash_algo_t;

/**
 * Computes the hash of a given data buffer using the specified algorithm.
 *
 * This implementation supports arbitrary bit lengths. If bit_len is not a multiple
 * of 8, the most significant bits of the last byte in 'data' are processed.
 * (e.g., for bit_len=1, the MSB of data[0] is hashed).
 *
 * @param algo The hash algorithm to use.
 * @param data The data to hash.
 * @param bit_len The length of the data in BITS.
 * @param digest The buffer to store the resulting hash.
 * @return 1 on success, 0 on failure.
 */
int hash_compute(const hash_algo_t* algo, const void* data, size_t bit_len, uint8_t* digest);

/**
 * Computes the hash of a given data buffer and returns only the first HASH_OUTPUT_SIZE bits.
 *
 * Supports arbitrary bit lengths for the input message.
 *
 * @param algo The hash algorithm to use.
 * @param data The data to hash.
 * @param bit_len The length of the data in BITS.
 * @param digest The buffer to store the resulting truncated hash.
 * @return 1 on success, 0 on failure.
 */
int hash_compute_max_bits(const hash_algo_t* algo, const void* data, size_t bit_len, uint8_t* digest);

#endif /* HASH_H */
