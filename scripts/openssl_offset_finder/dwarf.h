/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of parsing Debugging With Arbitrary
 * Record Formats (DWARF) debug information
 */

#ifndef OPENSSL_OFFSET_DWARF_H
#define OPENSSL_OFFSET_DWARF_H

#include <stddef.h>
#include <stdint.h>

/** Maximum number of DWARF line-number entries collected per function. */
#define DW_MAX_ENTRIES 256

/**
 * Decodes a ULEB128 (unsigned little-endian base-128) value.
 *
 * @param p Pointer to the byte stream; advanced past the encoded value.
 * @return The decoded unsigned value.
 */
static inline unsigned long read_uleb(const unsigned char** p) {
  unsigned long val = 0;
  int shift = 0;
  while (1) {
    unsigned char b = *(*p)++;
    val |= (unsigned long)(b & 0x7f) << shift;
    if (!(b & 0x80)) return val;
    shift += 7;
  }
}

/**
 * Decodes a SLEB128 (signed little-endian base-128) value.
 *
 * @param p Pointer to the byte stream; advanced past the encoded value.
 * @return The decoded signed value.
 */
static inline long read_sleb(const unsigned char** p) {
  long val = 0;
  int shift = 0;
  unsigned char b;
  do {
    b = *(*p)++;
    val |= (long)(b & 0x7f) << shift;
    shift += 7;
  } while (b & 0x80);
  if (shift < (long)sizeof(long) * 8 && (b & 0x40))
    val |= -(1L << shift);
  return val;
}

/**
 * Reads @p n little-endian bytes into an unsigned long.
 *
 * @param p Pointer to the byte stream; advanced by n bytes.
 * @param n Number of bytes to read (1-8).
 * @return The assembled unsigned value.
 */
static inline unsigned long read_bytes_le(const unsigned char** p, int n) {
  unsigned long val = 0;
  for (int i = 0; i < n; i++)
    val |= (unsigned long)(*(*p)++) << (i * 8);
  return val;
}

/**
 * @struct dw_entry_t
 * Entry collected from the DWARF line-number program.
 *
 * @var addr Virtual address of the instruction.
 * @var line Source line number.
 */
typedef struct {
  unsigned long addr;
  int line;
} dw_entry_t;

/**
 * Finds the address of the last (highest-numbered) source line in a
 * function by parsing the .debug_line section (epilogue/return line).
 *
 * @param dbg_fd     Open file descriptor to the debug-info ELF file.
 * @param func_vaddr Virtual address of the function start.
 * @param func_size  Size of the function in bytes.
 * @param out_vaddr  Output for the virtual address of the return line.
 * @return 0 on success, -1 on failure.
 */
int find_return_line_via_dwarf(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long* out_vaddr);

/**
 * Finds the first (lowest) address of the source line containing a given
 * instruction address, by parsing the .debug_line section.
 *
 * @param dbg_fd Open file descriptor to the debug-info ELF file.
 * @param func_vaddr Virtual address of the function start.
 * @param func_size Size of the function in bytes.
 * @param insn_vaddr Virtual address of the instruction to resolve.
 * @param line_vaddr Output for the first address of the source line.
 * @return 0 on success, -1 on failure.
 */
int find_line_start_via_dwarf(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long insn_vaddr, unsigned long* line_vaddr);

/**
 * Returns the source line number for a given instruction address
 * in a function, by parsing the .debug_line section.
 *
 * @param dbg_fd Open file descriptor to the debug-info ELF file.
 * @param func_vaddr Virtual address of the function start.
 * @param func_size Size of the function in bytes.
 * @param insn_vaddr Virtual address of the instruction to look up.
 * @param out_line Output for the source line number.
 * @return 0 on success, -1 on failure.
 */
int find_line_number_for_addr(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long insn_vaddr, int* out_line);

#endif /* OPENSSL_OFFSET_DWARF_H */
