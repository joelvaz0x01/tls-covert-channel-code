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
    fprintf(stderr, "open failed\n");
    return 1;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(stderr, "fstat failed\n");
    close(fd);
    return 1;
  }

  size_t data_size = st.st_size;
  if (data_size == 0) {
    fprintf(stderr, "data_size is 0\n");
    close(fd);
    return 1;
  }

  unsigned char* mem = malloc(16 + data_size);
  if (NULL == mem) {
    fprintf(stderr, "malloc failed\n");
    close(fd);
    return 1;
  }
  memset(mem, 0, 16 + data_size);

  unsigned long long data_size_to_ull = (unsigned long long)data_size;
  memcpy(mem, &data_size_to_ull, sizeof(data_size_to_ull));

  ssize_t n = read(fd, mem + 16, data_size);
  close(fd);

  if (n < 0 || (size_t)n != data_size) {
    fprintf(stderr, "read failed\n");
    free(mem);
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
