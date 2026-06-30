/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of parsing Debugging With Arbitrary
 * Record Formats (DWARF) debug information
 */

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dwarf.h"
#include "elf.h"

static int collect_line_entries(int dbg_fd, unsigned long func_vaddr, size_t func_size, dw_entry_t* entries, int* n_out) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(dbg_fd, &ehdr)) return -1;

  Elf64_Shdr debug_line;
  if (find_section(dbg_fd, &ehdr, ".debug_line", &debug_line) != 0)
    return -1;

  unsigned char* data = malloc(debug_line.sh_size);
  if (!data) return -1;
  lseek(dbg_fd, debug_line.sh_offset, SEEK_SET);
  if (read(dbg_fd, data, debug_line.sh_size) != (ssize_t)debug_line.sh_size) {
    free(data);
    return -1;
  }

  const unsigned char* p = data;
  const unsigned char* end = data + debug_line.sh_size;
  unsigned long func_end = func_vaddr + func_size;

  int n = 0;
  int target_file = -1;

  while (p < end) {
    if (p + 4 > end) break;
    uint32_t unit_length = read_bytes_le(&p, 4);
    if (unit_length == 0xffffffffU) break;
    const unsigned char* unit_end = p + unit_length - 4;
    if (unit_end > end) break;
    if (p + 2 > unit_end) break;
    uint16_t version = read_bytes_le(&p, 2);

    int address_size = 4;
    uint8_t min_inst_len = 1;
    uint8_t default_is_stmt = 1;
    int8_t line_base = -5;
    uint8_t line_range = 14;
    uint8_t opcode_base = 13;
    const unsigned char* line_prog = NULL;

    if (version == 4) {
      if (p + 4 > unit_end) break;
      uint32_t hl = read_bytes_le(&p, 4);
      line_prog = p + hl;
      if (line_prog > unit_end) break;
      if (p + 5 > line_prog) break;
      min_inst_len = *p++;
      default_is_stmt = *p++;
      line_base = (int8_t)(*p++);
      line_range = *p++;
      opcode_base = *p++;
    } else if (version >= 5) {
      address_size = *p++;
      uint8_t seg_sel_size = *p++;
      (void)seg_sel_size;
      if (p + 4 > unit_end) break;
      uint32_t hl = read_bytes_le(&p, 4);
      line_prog = p + hl;
      if (line_prog > unit_end) break;
      if (p + 6 > line_prog) break;
      min_inst_len = *p++;
      (void)*p++; /* max_ops_per_inst */
      default_is_stmt = *p++;
      line_base = (int8_t)(*p++);
      line_range = *p++;
      opcode_base = *p++;
    } else {
      break;
    }

    if (opcode_base > 1) {
      unsigned skip = opcode_base - 1;
      if (p + skip > line_prog) break;
      p += skip;
    }
    p = line_prog;

    unsigned long address = 0;
    unsigned long file = 1;
    unsigned long cur_line = 1;
    int is_stmt = default_is_stmt;

    while (p < unit_end) {
      uint8_t opcode = *p++;
      int do_emit = 0;

      if (opcode == 0) {
        if (p >= unit_end) break;
        uint8_t len = *p++;
        const unsigned char* ext_end = p + len;
        if (ext_end > unit_end) break;
        uint8_t sub = *p++;

        switch (sub) {
          case 0x01:
            address = 0;
            file = 1;
            cur_line = 1;
            is_stmt = default_is_stmt;
            break;
          case 0x02:
            address = read_bytes_le(&p, address_size);
            break;
          case 0x03:
            break;
          case 0x04:
            (void)read_uleb(&p);
            break;
        }
        p = ext_end;
        continue;
      }

      if (opcode < opcode_base) {
        switch (opcode) {
          case 0x01:
            do_emit = 1;
            break;
          case 0x02:
            address += read_uleb(&p) * min_inst_len;
            break;
          case 0x03:
            cur_line += read_sleb(&p);
            break;
          case 0x04:
            file = read_uleb(&p);
            break;
          case 0x05:
            (void)read_uleb(&p);
            break;
          case 0x06:
            is_stmt = !is_stmt;
            break;
          case 0x07:
            break;
          case 0x08:
            address += ((255U - opcode_base) / line_range) * min_inst_len;
            break;
          case 0x09:
            address += read_bytes_le(&p, 2);
            break;
          case 0x0a:
            break;
          case 0x0b:
            break;
          case 0x0c:
            (void)read_uleb(&p);
            break;
        }
      } else {
        unsigned adj = opcode - opcode_base;
        address += (adj / line_range) * min_inst_len;
        cur_line += line_base + (long)(adj % line_range);
        do_emit = 1;
      }

      if (do_emit) {
        if (address >= func_vaddr && address < func_end) {
          if (target_file < 0)
            target_file = (int)file;
          if ((int)file == target_file && n < DW_MAX_ENTRIES) {
            entries[n].addr = address;
            entries[n].line = (int)cur_line;
            n++;
          }
        }
      }
    }
  }

  free(data);
  *n_out = n;
  return n > 0 ? 0 : -1;
}

int find_return_line_via_dwarf(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long* out_vaddr) {
  dw_entry_t entries[DW_MAX_ENTRIES];
  int n = 0;
  if (collect_line_entries(dbg_fd, func_vaddr, func_size, entries, &n) != 0)
    return -1;

  int highest = entries[0].line;
  unsigned long result = entries[0].addr;
  for (int i = 0; i < n; i++) {
    if (entries[i].line > highest) {
      highest = entries[i].line;
      result = entries[i].addr;
    }
  }

  *out_vaddr = result;
  return 0;
}

int find_line_start_via_dwarf(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long insn_vaddr, unsigned long* line_vaddr) {
  dw_entry_t entries[DW_MAX_ENTRIES];
  int n = 0;
  if (collect_line_entries(dbg_fd, func_vaddr, func_size, entries, &n) != 0)
    return -1;

  int target_line = -1;
  for (int i = 0; i < n; i++) {
    if (entries[i].addr <= insn_vaddr)
      target_line = entries[i].line;
  }
  if (target_line < 0) return -1;

  for (int i = 0; i < n; i++) {
    if (entries[i].line == target_line) {
      *line_vaddr = entries[i].addr;
      return 0;
    }
  }

  return -1;
}

int find_line_number_for_addr(int dbg_fd, unsigned long func_vaddr, size_t func_size, unsigned long insn_vaddr, int* out_line) {
  dw_entry_t entries[DW_MAX_ENTRIES];
  int n = 0;
  if (collect_line_entries(dbg_fd, func_vaddr, func_size, entries, &n) != 0)
    return -1;

  for (int i = n - 1; i >= 0; i--) {
    if (entries[i].addr <= insn_vaddr) {
      *out_line = entries[i].line;
      return 0;
    }
  }

  return -1;
}
