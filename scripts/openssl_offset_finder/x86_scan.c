/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of finding call instructions
 * in x86_64 machine code. Supports direct call
 * instructions (E8 rel32) and indirect call
 * instructions (FF 15 call [rip+disp32]).
 */

#include <stdlib.h>
#include <unistd.h>

#include "elf.h"
#include "x86_scan.h"

int find_nth_call_to_target(int lib_fd, unsigned long func_vaddr, size_t func_size, unsigned long target_vaddr, int n, unsigned long* out_call_vaddr) {
  unsigned long func_foff = 0;
  if (vaddr_to_file_offset(lib_fd, func_vaddr, &func_foff) != 0)
    return -1;

  unsigned char* code = malloc(func_size);
  if (!code) return -1;

  lseek(lib_fd, func_foff, SEEK_SET);
  if (read(lib_fd, code, func_size) != (ssize_t)func_size) {
    free(code);
    return -1;
  }

  int count = 0;
  size_t i = 0;
  while (i < func_size) {
    unsigned long call_vaddr = 0;

    if (i + 5 <= func_size && code[i] == 0xE8) {
      int rel32 = (int)(code[i + 1]) |
                  (int)(code[i + 2] << 8) |
                  (int)(code[i + 3] << 16) |
                  (int)(code[i + 4] << 24);
      call_vaddr = func_vaddr + i + 5 + rel32;
      if (call_vaddr == target_vaddr) {
        count++;
        if (count == n) {
          *out_call_vaddr = func_vaddr + i;
          free(code);
          return 0;
        }
      }
      i += 5;
    } else if (i + 6 <= func_size && code[i] == 0xFF && code[i + 1] == 0x15) {
      int disp32 = (int)(code[i + 2]) |
                   (int)(code[i + 3] << 8) |
                   (int)(code[i + 4] << 16) |
                   (int)(code[i + 5] << 24);
      call_vaddr = func_vaddr + i + 6 + disp32;
      if (call_vaddr == target_vaddr) {
        count++;
        if (count == n) {
          *out_call_vaddr = func_vaddr + i;
          free(code);
          return 0;
        }
      }
      i += 6;
    } else {
      i++;
    }
  }

  free(code);
  return -1;
}

int find_all_calls_to_target(int lib_fd, unsigned long func_vaddr, size_t func_size, unsigned long target_vaddr, unsigned long* out_calls, int max_calls) {
  unsigned long func_foff = 0;
  if (vaddr_to_file_offset(lib_fd, func_vaddr, &func_foff) != 0)
    return -1;

  unsigned char* code = malloc(func_size);
  if (!code) return -1;

  lseek(lib_fd, func_foff, SEEK_SET);
  if (read(lib_fd, code, func_size) != (ssize_t)func_size) {
    free(code);
    return -1;
  }

  int count = 0;
  size_t i = 0;
  while (i < func_size && count < max_calls) {
    if (i + 5 <= func_size && code[i] == 0xE8) {
      int rel32 = (int)(code[i + 1]) |
                  (int)(code[i + 2] << 8) |
                  (int)(code[i + 3] << 16) |
                  (int)(code[i + 4] << 24);
      unsigned long call_target = func_vaddr + i + 5 + rel32;
      if (call_target == target_vaddr) {
        out_calls[count++] = func_vaddr + i;
      }
      i += 5;
    } else if (i + 6 <= func_size && code[i] == 0xFF && code[i + 1] == 0x15) {
      int disp32 = (int)(code[i + 2]) |
                   (int)(code[i + 3] << 8) |
                   (int)(code[i + 4] << 16) |
                   (int)(code[i + 5] << 24);
      unsigned long call_target = func_vaddr + i + 6 + disp32;
      if (call_target == target_vaddr) {
        out_calls[count++] = func_vaddr + i;
      }
      i += 6;
    } else {
      i++;
    }
  }

  free(code);
  return count;
}
