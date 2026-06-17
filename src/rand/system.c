/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdint.h>

#ifdef _WIN32
#include <bcrypt.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "rand128.h"
#include "rand64.h"
#include "system.h"
#include "time.h"

static inline int get_system_seed(void* s, size_t len) {
#ifdef _WIN32
  return 0 == BCryptGenRandom(NULL, (PUCHAR)s, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd != -1) {
    ssize_t n = read(fd, s, len);
    close(fd);
    return n == (ssize_t)len;
  }
  return 0;
#endif
}

void seed64_system(void) {
  uint64_t s = 0;

  if (!get_system_seed(&s, sizeof(s))) {
    seed64_time();
    return;
  }

  srand64(s);
}

void seed128_system(void) {
  uint128_t s = {0, 0};

  if (!get_system_seed(&s, sizeof(s))) {
    seed128_time();
    return;
  }

  srand128(s);
}
