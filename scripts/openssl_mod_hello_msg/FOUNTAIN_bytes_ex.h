/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_BYTES_EX_H
#define FOUNTAIN_BYTES_EX_H

#include <openssl/rand.h>

extern unsigned char* volatile shm_base;

int FOUNTAIN_bytes_ex(OSSL_LIB_CTX* ctx, unsigned char* buf, size_t num, unsigned int strength);

#endif /* FOUNTAIN_BYTES_EX_H */
