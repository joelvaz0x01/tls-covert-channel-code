/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FC_DECODE_H
#define FC_DECODE_H

#include <stdint.h>

#include <cbprng/cbprng.h>

#include <fountain_code/utils.h>

#include "utils.h"

#if USE_SYSTEM_RANDOM
#include <rand/system.h>
#else
#include <rand/rand128.h>
#endif

extern generator_t cbprng;   /* cbprng state             */
extern mask_t counter_value; /* counter value for cbprng */

/**
 * Initializes the CBPRNG state.
 */
static inline void init_cbprng(void) {
#if USE_SYSTEM_RANDOM
  seed128_system();
#else
  srand128(42);
#endif
  pseudo_random_generator(&cbprng);
}

/**
 * The final modified TLS random field.
 *
 * @param id File ID.
 * @param m Number of blocks to encode.
 * @param n Number of indices to generate.
 * @param n_words Number of words in the message.
 * @param src_file Filename of the source file.
 * @return Modified random field.
 */
tls_mod_rand_t modified_random_field(const uint64_t id, uint64_t* seed, const uint64_t m, const uint64_t n, const uint64_t n_words, const char* src_file);

#endif /* FC_DECODE_H */
