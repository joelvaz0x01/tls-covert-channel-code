/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <stdint.h>

#include <cbprng/cbprng.h>

#include <fountain_code/utils.h>

#include "settings.h"

#if USE_SYSTEM_RANDOM
#include <rand/system.h>
#else
#include <rand/rand128.h>
#endif

extern decoder_t* dec;          /* decoder state                    */
extern block_t* buffer;         /* buffer that holds only k blocks  */
extern uint64_t* k_list;        /* list of k indices for each block */
extern packet_t* g_scratch_pkt; /* scratch packet for hot path      */

/**
 * @struct tls_mod_rand_t
 * Holds the result of a TLS modulus random number generation.
 *
 * @var cbprng The random number generator state.
 * @var fountain_code The fountain code block.
 * @var hash The hash value of the random number generation result.
 */
typedef struct {
  mask_t cbprng;
  block_t enc_fc;
  uint32_t hash;
} tls_mod_rand_t;

/**
 * Calculates the number of file part based on the file size.
 *
 * @param filename The name of the file.
 * @param is_encoder If the caller is the encoder.
 * @return The number of file parts.
 */
uint64_t calculate_n(const char* filename, const bool is_encoder);

/**
 * Initializes the program state.
 *
 * @param n The number of blocks in the fountain code.
 * @param m The size of the output buffer.
 * @return The number of words for the Fountain Code block.
 */
uint64_t init_program(uint64_t n, uint64_t m);

/**
 * Finalizes the program state.
 */
void finalize_program(void);

/**
 * Constructs the k value for the Fountain Code.
 *
 * @param seed The seed for the random number generator.
 * @param m The number of blocks in the Fountain Code.
 * @param n The number of words in the Fountain Code.
 * @param n_words The number of words in the scratch buffer.
 * @param src_file The source file to read from.
 * @return The k value on success, 0 on failure if src_file is not NULL.
 */
uint64_t construct_k(const uint64_t seed, const uint64_t m, const uint64_t n, const uint64_t n_words, const char* src_file);

/**
 * Cypher the Fountain Code.
 *
 * @param fc The fountain code to cypher.
 * @param seed The CBPRNG based seed.
 * @param file_id The file ID.
 */
void cypher_fountain(block_t* fc, const uint64_t seed, const uint64_t file_id);

/**
 * Builds the hash of the encrypted Fountain Code.
 *
 * @param enc_b The encrypted fountain code.
 * @param seed The CBPRNG based seed.
 * @param file_id The file ID.
 * @param is_valid The validity of the data.
 * @param digest_out The output digest.
 */
void build_hash(const block_t* enc_b, uint64_t seed, const uint64_t file_id, const char* is_valid, uint32_t* digest_out);

#endif /* UTILS_UTILS_H */
