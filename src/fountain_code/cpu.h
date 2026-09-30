/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Streaming SIMD Extensions 4 (SSE4)
 * detection made with AI assistance
 */

#ifndef FOUNTAIN_CODE_CPU_H
#define FOUNTAIN_CODE_CPU_H

#include <stdbool.h>

#if defined(__x86_64__) || defined(_M_AMD64)

#if defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#elif defined(_MSC_VER)
#include <intrin.h>
#endif

/**
 * Check if the CPU supports SSE4.2 instructions.
 *
 * @return true if SSE4.2 is supported, false otherwise.
 */
static inline bool cpu_has_sse4_2(void) {
#if defined(__GNUC__) || defined(__clang__)
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  if (__get_cpuid(1, &eax, &ebx, &ecx, &edx))
    return (ecx >> 20) & 1;
  return false;
#elif defined(_MSC_VER)
  int info[4];
  __cpuid(info, 1);
  return (info[2] >> 20) & 1;
#else
  return false;
#endif
}

#else

static inline bool cpu_has_sse4_2(void) {
  return false;
}

#endif

#endif /* FOUNTAIN_CODE_CPU_H */
