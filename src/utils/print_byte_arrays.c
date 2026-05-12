/**
 * @file print_byte_arrays.c
 * @brief Utility functions for printing byte arrays.
 */

#include <stdio.h>

#include <utils/print_byte_arrays.h>

void print_hex(const unsigned char *d, int len)
{
  for (int i = 0; i < len; i++) printf("%02x", d[i]);
}

void print_ascii(const unsigned char *d, int len)
{
  for (int i = 0; i < len; i++) {
    unsigned char c = d[i];
    putchar((c >= 32 && c < 127) ? (char)c : '.');
  }
}
