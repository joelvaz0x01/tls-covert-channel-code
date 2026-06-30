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

int main(void) {
  int fd = open("./data.bin", O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  struct stat st;
  fstat(fd, &st);
  size_t file_size = st.st_size;

  unsigned char* mem = malloc(8 + 4096);
  if (!mem) {
    perror("malloc");
    return 1;
  }
  memset(mem, 0, 8 + 4096);

  ssize_t n = read(fd, mem + 8, file_size < 4088 ? file_size : 4088);
  close(fd);
  if (n <= 0 && file_size > 0) {
    perror("read");
    return 1;
  }

  shm_base = mem;

  unsigned char buf[32];
  while (1) {
    memset(buf, 0, 32);
    if (1 == FOUNTAIN_bytes_ex(NULL, buf, 32, 0)) {
      for (int i = 0; i < 32; i++)
        printf("%02x", buf[i]);
      printf("\n");
    }
    sleep(1);
  }
  return 0;
}
