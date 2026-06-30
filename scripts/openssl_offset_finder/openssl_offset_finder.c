/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of finding offsets on
 * ssl_fill_hello_random() in the libssl binary.
 *
 * Locates an address inside ssl_fill_hello_random() relative to the
 * libssl base in memory, by parsing ELF, /proc/self/maps, and either
 * DWARF .debug_line (RET_LINE mode) or x86_64 machine code scanning +
 * DWARF (default mode, finds the N-th call to RAND_bytes_ex).
 *
 * This file supports the following build modes:
 *   - RET_LINE=0: x86 scan (default)
 *   - RET_LINE=1: DWARF
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elf.h"
#include "openssl_detect.h"

#ifdef RET_LINE
#include "dwarf.h"
#else
#include "dwarf.h"
#include "x86_scan.h"
#define RAND_FUNC      "RAND_bytes_ex"
#define RAND_CALL      2
#define MAX_RAND_CALLS 32
#endif

#define RED         "\033[0;31m"
#define GREEN       "\033[0;32m"
#define BOLD        "\033[1m"
#define NC          "\033[0m"
#define FUNC_PREFIX "ssl_fill_hello_random"

static void die(const char* msg) {
  fprintf(stderr, RED "[ERROR]" NC " %s\n", msg);
  exit(1);
}

typedef struct {
  unsigned long addr;
  unsigned long file_off;
} map_entry;

static int get_rxp_entry(map_entry* e) {
  FILE* f = fopen("/proc/self/maps", "r");
  if (!f) return -1;

  char line[4096];
  while (fgets(line, sizeof(line), f)) {
    if (!strstr(line, "r-xp")) continue;
    if (!strstr(line, "libssl.so")) continue;

    unsigned long start = 0, off = 0, end = 0;
    unsigned int dev_maj = 0, dev_min = 0;
    unsigned long inode = 0;
    char path_buf[1024] = {0};
    if (sscanf(line, "%lx-%lx r-xp %lx %x:%x %lu %1023s", &start, &end, &off, &dev_maj, &dev_min, &inode, path_buf) >= 3) {
      e->addr = start;
      e->file_off = off;
      fclose(f);
      return 0;
    }
  }
  fclose(f);
  return -1;
}

int main(void) {
  const char* lib_path = NULL;
  void* handle = detect_openssl(&lib_path);
  if (!handle) die("OpenSSL not found. Please install OpenSSL.");
  if (!lib_path) lib_path = "libssl.so";
  printf(GREEN "[OK]" NC " OpenSSL detected (%s)\n", lib_path);

  char dbg_path[4096];
  if (find_debug_path(lib_path, dbg_path, sizeof(dbg_path)) != 0 || !debug_file_has_symtab(dbg_path))
    die("Please install the debug symbols on your system");
  printf(GREEN "[OK]" NC " Debug symbols found: %s\n", dbg_path);

  int lib_fd_bid = open(lib_path, O_RDONLY);
  int dbg_fd_bid = open(dbg_path, O_RDONLY);
  unsigned char lib_id[64], dbg_id[64];
  size_t lib_id_len = sizeof(lib_id), dbg_id_len = sizeof(dbg_id);
  int lib_bid_ok = (lib_fd_bid >= 0 && get_build_id(lib_fd_bid, lib_id, &lib_id_len) == 0);
  int dbg_bid_ok = (dbg_fd_bid >= 0 && get_build_id(dbg_fd_bid, dbg_id, &dbg_id_len) == 0);
  if (lib_fd_bid >= 0) close(lib_fd_bid);
  if (dbg_fd_bid >= 0) close(dbg_fd_bid);

  if (lib_bid_ok && dbg_bid_ok) {
    if (lib_id_len != dbg_id_len || memcmp(lib_id, dbg_id, lib_id_len) != 0) {
      char* ver = get_openssl_version();
      char err[512];
      snprintf(err, sizeof(err), "Please install the OpenSSL symbols for version %s", ver);
      free(ver);
      die(err);
    }
  }

  map_entry e;
  if (get_rxp_entry(&e) != 0)
    die("Cannot find libssl.so r-xp mapping in /proc/self/maps");
  printf(GREEN "[OK]" NC " libssl.so r-xp base: 0x%lX  (file offset: 0x%lX)\n", e.addr, e.file_off);

  int dbg_fd = open(dbg_path, O_RDONLY);
  if (dbg_fd < 0) die("Cannot open debug file");

  unsigned long func_vaddr = 0, func_size = 0;
  if (find_function_in_symtab(dbg_fd, FUNC_PREFIX, &func_vaddr, &func_size) != 0) {
    close(dbg_fd);
    die("Symbol " FUNC_PREFIX "* not found in debug symbols");
  }
  printf(GREEN "[OK]" NC " %s @ vaddr 0x%lX, size %zu\n", FUNC_PREFIX, func_vaddr, func_size);

  int lib_fd = open(lib_path, O_RDONLY);
  if (lib_fd < 0) {
    close(dbg_fd);
    die("Cannot open libssl.so");
  }

#ifdef RET_LINE
  unsigned long target_vaddr = 0;
  if (find_return_line_via_dwarf(dbg_fd, func_vaddr, func_size, &target_vaddr) != 0) {
    close(dbg_fd);
    close(lib_fd);
    die("Cannot locate final return line in DWARF .debug_line");
  }

  unsigned long target_foff = 0;
  if (vaddr_to_file_offset(lib_fd, target_vaddr, &target_foff) != 0) {
    close(dbg_fd);
    close(lib_fd);
    die("Cannot convert return vaddr to file offset");
  }
  printf(GREEN "[OK]" NC " Final return line @ vaddr 0x%lX  (file offset 0x%lX)\n", target_vaddr, target_foff);
#else
  unsigned long rand_target = 0;
  const char* target_type = NULL;
  if (find_got_entry(lib_fd, RAND_FUNC, &rand_target) == 0) {
    target_type = "GOT entry";
  } else if (find_plt_entry(lib_fd, RAND_FUNC, &rand_target) == 0) {
    target_type = "PLT entry";
  } else {
    close(dbg_fd);
    close(lib_fd);
    die("Cannot find GOT or PLT entry for " RAND_FUNC " in libssl.so");
  }
  printf(GREEN "[OK]" NC " %s %s @ vaddr 0x%lX\n", RAND_FUNC, target_type, rand_target);

  unsigned long call_vaddrs[MAX_RAND_CALLS];
  int n_calls = find_all_calls_to_target(lib_fd, func_vaddr, func_size, rand_target, call_vaddrs, MAX_RAND_CALLS);
  if (n_calls < RAND_CALL) {
    char err[512];
    snprintf(err, sizeof(err), "Found only %d call(s) to %s in %s, need at least %d", n_calls, RAND_FUNC, FUNC_PREFIX, RAND_CALL);
    close(dbg_fd);
    close(lib_fd);
    die(err);
  }

  int lines[MAX_RAND_CALLS];
  for (int i = 0; i < n_calls; i++) {
    if (find_line_number_for_addr(dbg_fd, func_vaddr, func_size, call_vaddrs[i], &lines[i]) != 0) {
      close(dbg_fd);
      close(lib_fd);
      die("Cannot determine source line for a call instruction via DWARF");
    }
  }

  for (int i = 0; i < n_calls - 1; i++) {
    for (int j = i + 1; j < n_calls; j++) {
      if (lines[i] > lines[j]) {
        int tmp_line = lines[i];
        lines[i] = lines[j];
        lines[j] = tmp_line;
        unsigned long tmp_addr = call_vaddrs[i];
        call_vaddrs[i] = call_vaddrs[j];
        call_vaddrs[j] = tmp_addr;
      }
    }
  }

  unsigned long target_vaddr = call_vaddrs[RAND_CALL - 1];
  unsigned long line_vaddr = 0;
  if (find_line_start_via_dwarf(dbg_fd, func_vaddr, func_size, target_vaddr, &line_vaddr) != 0) {
    close(dbg_fd);
    close(lib_fd);
    die("Cannot find source line for call instruction via DWARF");
  }
  target_vaddr = line_vaddr;

  unsigned long target_foff = 0;
  if (vaddr_to_file_offset(lib_fd, target_vaddr, &target_foff) != 0) {
    close(dbg_fd);
    close(lib_fd);
    die("Cannot convert call vaddr to file offset");
  }
  printf(GREEN "[OK]" NC " Call #%d to %s (source-line order) -> line start @ vaddr 0x%lX  (file offset 0x%lX)\n", RAND_CALL, RAND_FUNC, target_vaddr, target_foff);
#endif

  close(dbg_fd);
  close(lib_fd);

  unsigned long result = target_foff - e.file_off;
  printf("\n" BOLD "[RESULT]" NC " %s - libssl_base = 0x%lX  (%lu)\n",
#ifdef RET_LINE
         "return_address",
#else
         "rand_call_line_start",
#endif
         result,
         result);

  dlclose(handle);
  return 0;
}
