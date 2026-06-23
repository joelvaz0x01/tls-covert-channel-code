/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <cbprng/settings.h>
#include <fountain_code/settings.h>
#include <hash/settings.h>

/* by default use system random to seed CBPRNG */
#ifndef USE_SYSTEM_RANDOM
#define USE_SYSTEM_RANDOM 1
#endif

/* by default use SHA-256 for hashing */
#ifndef HASH_ALGORITHM
#include <hash/sha256.h>
#define HASH_ALGORITHM SHA256
#endif

#define KEY        "MY_SUPER_SECRET_KEY_FOR_MODIFIED_TLS_2026"
#define KEY_LEN    ((sizeof(KEY) - 1) * 8)

#define CBPRNG_LEN CBPRNG_BITS
#define FC_LEN     FC_BLOCK_SIZE
#define HASH_LEN   HASH_OUTPUT_SIZE

#if (CBPRNG_BITS + FC_BLOCK_SIZE + HASH_OUTPUT_SIZE != 256)
#error "Random field must be 256 bits (32 bytes)"
#endif

#define FC_LEN_BYTES (FC_LEN / 8)

#endif /* SETTINGS_H */
