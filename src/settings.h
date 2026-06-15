/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <cbprng/settings.h>
#include <fountain_code/settings.h>
#include <hash/settings.h>

#define CBPRNG_LEN CBPRNG_BITS
#define FC_LEN     FC_BLOCK_SIZE
#define HASH_LEN   HASH_OUTPUT_SIZE

#if (CBPRNG_LEN + FC_LEN + HASH_LEN != 256)
#error "Random field must be 256 bits (32 bytes)"
#endif



#endif /* SETTINGS_H */
