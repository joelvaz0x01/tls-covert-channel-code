/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Toy example adapted from Tomás Oliveira e Silva, May 2026
 */

#include <stdio.h>

#if defined(__clang__)
__attribute__((__noinline__, __optnone__))
#elif defined(__GNUC__)
__attribute__((__noinline__, __noipa__))
#else
__attribute__((__noinline__))
#endif
void original_function(void* addr, int count) {
  __asm__ __volatile__(
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop"
  );
  static unsigned long x = 127326573ul;
  for (char* p = (char*)addr; count > 0; count--) {
    *p++ = (char)x;
    x = x * 218372736138261ul + 17ul;
  }
}

int main(void) {
  char buffer[32];
  int k;

  for (int j = 1;;) {
    for (k = 0; k < 1000000000; k++)
      j = 3 * j - 1;
    original_function((void*)buffer, 4);
    for (k = 0; k < 4; k++)
      printf("%02X", (int)buffer[k] & 0xFF);
    printf(" -- %08X -- ", j);
    original_function((void*)buffer, 32);
    for (k = 0; k < 32; k++)
      printf("%02X", (int)buffer[k] & 0xFF);
    printf("\n");
  }
}
