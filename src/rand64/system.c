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

#include "rand64.h"
#include "system.h"
#include "time.h"

void seed64_system(void) {
  uint64_t seed = 0;
  int success = 0;

#ifdef _WIN32
  if (0 == BCryptGenRandom(NULL, (BYTE*)&seed, sizeof(seed), BCRYPT_USE_SYSTEM_PREFERRED_RNG)) {
    success = 1;
  }
#else
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd != -1) {
    if (read(fd, &seed, sizeof(seed)) == (ssize_t)sizeof(seed)) {
      success = 1;
    }
    close(fd);
  }
#endif

  if (!success) { /* fallback to time-based seed */
    seed64_time();
    return;
  }

  srand64(seed);
}
