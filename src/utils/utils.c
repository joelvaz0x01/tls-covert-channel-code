/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>

#include "utils.h"

void print_hex_bits(const void* d, int bits) {
  const unsigned char* p = (const unsigned char*)d;
  for (int b = 0; b < bits; b += 8) {
    printf("%02x", p[b >> 3]);
  }
}

void print_ascii_bits(const void* d, int bits) {
  const unsigned char* p = (const unsigned char*)d;
  for (int b = 0; b < bits; b += 8) {
    unsigned char c = p[b >> 3];
    putchar((c >= 32 && c < 127) ? (char)c : '.');
  }
}
