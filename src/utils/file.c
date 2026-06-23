/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fountain_code/utils.h>

#include "file.h"
#include "settings.h"
#include "utils.h"

static FILE* fp_src = NULL;
static FILE* fp_dst = NULL;

static long src_size = 0;

uint64_t open_file(const char* filename, const int is_src) {
  const char* mode = is_src ? "rb" : "wb";
  FILE** fp = is_src ? &fp_src : &fp_dst;

#ifdef _WIN32
  if (0 != fopen_s(fp, filename, mode)) return 0;
#else
  *fp = fopen(filename, mode);
  if (NULL == *fp) return 0;
#endif

  if (fseek(*fp, 0, SEEK_END) != 0) {
    fclose(*fp);
    *fp = NULL;
    return 0;
  }

  if (is_src) {
    src_size = ftell(*fp);
    if (src_size <= 0) {
      fclose(*fp);
      *fp = NULL;
      return 0;
    }
  }

  if (fseek(*fp, 0, SEEK_SET) != 0) {
    fclose(*fp);
    *fp = NULL;
    return 0;
  }

  return 1;
}

void close_files(void) {
  if (NULL != fp_src) {
    fclose(fp_src);
    fp_src = NULL;
  }

  if (NULL != fp_dst) {
    fclose(fp_dst);
    fp_dst = NULL;
  }
}

uint64_t calculate_n(const char* filename) {
  if (NULL == fp_src) {
    if (0 == open_file(filename, 1)) return 0;
  }

  long pos = ftell(fp_src);
  if (fseek(fp_src, 0, SEEK_END) != 0) {
    close_files();
    return 0;
  }

  long size = ftell(fp_src);
  if (size <= 0) {
    close_files();
    return 0;
  }

  if (fseek(fp_src, pos, SEEK_SET) != 0) {
    close_files();
    return 0;
  }

  return ((uint64_t)size + FC_LEN_BYTES - 1) / FC_LEN_BYTES;
}

int read_file_part(const char* filename, const uint64_t file_part, block_t* buffer) {
  if (NULL == fp_src) {
    if (0 == open_file(filename, 1)) return -1;
  }

  long offset = (long)(file_part * FC_LEN_BYTES);
  if (offset >= src_size) {
    close_files();
    return -1;
  }

  if (0 != fseek(fp_src, offset, SEEK_SET)) {
    close_files();
    return -1;
  }

  size_t bytes_to_read = FC_LEN_BYTES;
  if ((size_t)(src_size - offset) < bytes_to_read)
    bytes_to_read = (size_t)(src_size - offset);

#ifdef _WIN32
  size_t n = fread_s(buffer, sizeof(block_t), 1, bytes_to_read, fp_src);
#else
  size_t n = fread(buffer, 1, bytes_to_read, fp_src);
#endif
  if (n != bytes_to_read) {
    close_files();
    return -1;
  }

  if (n < FC_LEN_BYTES)
    memset((uint8_t*)buffer + n, 0, FC_LEN_BYTES - n);

  return 0;
}

int save_fountain_code(const char* filename, const tls_mod_rand_t mod_rand) {
  if (NULL == fp_dst) {
    if (0 == open_file(filename, 0)) return -1;
  }

  write_fountain(mod_rand, fp_dst);
  return 0;
}
