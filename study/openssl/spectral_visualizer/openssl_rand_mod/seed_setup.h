/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance based on
 * OpenSSL 3.6.3 source to produce a deterministic RAND_bytes_ex
 */

#ifndef OPENSSL_RAND_MOD_SEED_SETUP_H
#define OPENSSL_RAND_MOD_SEED_SETUP_H

#include <openssl/types.h>

/*
 * Generate deterministic random bytes
 *
 * On first call, creates the deterministic DRBG chain that
 * replaces RAND_bytes_ex():
 *   1. DET-SRC (seed source)
 *   2. CTR-DRBG (AES-256-CTR)
 *
 * @param ctx  OSSL_LIB_CTX for algorithm lookup
 * @param buf  Output buffer
 * @param size Number of bytes to generate
 * @return 1 on success, 0 on failure
 */
int DET_RAND_bytes_ex(OSSL_LIB_CTX* ctx, unsigned char* buf, size_t size, unsigned int strength);

/*
 * Initialise the deterministic OpenSSL RNG environment
 *
 * Registers the "det" provider as a built-in and loads both the "det"
 * and "default" providers into the given library context.
 *
 * The DRBG chain is created lazily when DET_RAND_bytes_ex() is called
 * for the first time.
 *
 * @param ctx OSSL_LIB_CTX to configure
 * @return 1 on success, 0 on failure
 */
int DET_RAND_bytes_register(OSSL_LIB_CTX* ctx);

#endif /* OPENSSL_RAND_MOD_SEED_SETUP_H */
