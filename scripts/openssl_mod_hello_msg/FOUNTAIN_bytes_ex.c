/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * OpenSSL file 'ssl/s3_lib.c' imports 'internal/e_os.h'
 * header, which contains platform-specific definitions,
 * allowing functions like __atomic_* to be used.
 */

#include "FOUNTAIN_bytes_ex.h"

unsigned char* volatile shm_base;

int FOUNTAIN_bytes_ex(OSSL_LIB_CTX* ctx, unsigned char* buf, size_t num, unsigned int strength) {
  unsigned char* base = shm_base;
  if (NULL == base)
    return RAND_bytes_ex(ctx, buf, num, strength);

  /* data size (offset 0) */
  unsigned long long data_size = *(volatile unsigned long long*)base;

  /* file cursor (offset 8) */
  volatile unsigned long long* cnt = (volatile unsigned long long*)(base + 8);

  /* data (offset 16) */
  unsigned char* dat = base + 16;

  unsigned long long pos = __atomic_fetch_add(cnt, (unsigned long long)num, __ATOMIC_SEQ_CST);
  if (pos > data_size - (unsigned long long)num) {
    __atomic_store_n(cnt, (unsigned long long)-1, __ATOMIC_SEQ_CST);
    return RAND_bytes_ex(ctx, buf, num, strength);
  }

  /* copy 'num' bytes from 'dat' to 'buf' */
  for (unsigned long long i = 0; i < (unsigned long long)num; i++)
    buf[i] = dat[pos + i];

  return 1;
}
