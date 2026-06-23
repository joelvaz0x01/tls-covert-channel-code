/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FC_DECODE_H
#define FC_DECODE_H

#include <stdint.h>

#include <utils/utils.h>

/**
 * The final modified TLS random field.
 *
 * @param id File ID.
 * @param m Number of blocks to encode.
 * @param n Number of indices to generate.
 * @param n_words Number of words in the message.
 * @return Modified random field.
 */
tls_mod_rand_t modified_random_field(const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words);

#endif /* FC_DECODE_H */
