/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This file has only created to test if FOUNTAIN_bytes_ex
 * works as expected before integrating it into OpenSSL.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FOUNTAIN_bytes_ex.h"

#define RAND_LEN 32

int main(void) {
  int fd = open("./data.bin", O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  struct stat st;
  fstat(fd, &st);
  size_t data_size = st.st_size;

  unsigned char* mem = malloc(16 + data_size);
  if (NULL == mem) {
    perror("malloc");
    return 1;
  }
  memset(mem, 0, 16 + data_size);
  *(unsigned long long*)mem = data_size;

  ssize_t n = read(fd, mem + 16, data_size);
  close(fd);
  if (n <= 0 && data_size > 0) {
    free(mem);
    mem = NULL;
    perror("read");
    return 1;
  }

  shm_base = mem;

  unsigned char buf[RAND_LEN];
  while (1) {
    memset(buf, 0, RAND_LEN);
    if (1 == FOUNTAIN_bytes_ex(NULL, buf, RAND_LEN, 0)) {
      for (int i = 0; i < RAND_LEN; i++)
        printf("%02x", buf[i]);
      printf("\n");
    }
    sleep(1);
  }
}
