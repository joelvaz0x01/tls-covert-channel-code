/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of parsing ELF
 */

#define _GNU_SOURCE

#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elf.h"

int read_elf_header(int fd, Elf64_Ehdr* ehdr) {
  lseek(fd, 0, SEEK_SET);
  return read(fd, ehdr, sizeof(*ehdr)) == sizeof(*ehdr) &&
         ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
         ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
         ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
         ehdr->e_ident[EI_MAG3] == ELFMAG3;
}

int find_section(int fd, const Elf64_Ehdr* ehdr, const char* name, Elf64_Shdr* out) {
  Elf64_Shdr shstrtab;
  lseek(fd, ehdr->e_shoff + ehdr->e_shstrndx * ehdr->e_shentsize, SEEK_SET);
  if (read(fd, &shstrtab, sizeof(shstrtab)) != sizeof(shstrtab))
    return -1;

  char* buf = malloc(shstrtab.sh_size);
  if (!buf) return -1;
  lseek(fd, shstrtab.sh_offset, SEEK_SET);
  if (read(fd, buf, shstrtab.sh_size) != (ssize_t)shstrtab.sh_size) {
    free(buf);
    return -1;
  }

  Elf64_Shdr shdr;
  int found = 0;
  for (int i = 0; i < ehdr->e_shnum; i++) {
    lseek(fd, ehdr->e_shoff + i * ehdr->e_shentsize, SEEK_SET);
    if (read(fd, &shdr, sizeof(shdr)) != sizeof(shdr)) break;
    if (strcmp(buf + shdr.sh_name, name) == 0) {
      *out = shdr;
      found = 1;
      break;
    }
  }
  free(buf);
  return found ? 0 : -1;
}

int find_debug_path(const char* lib_path, char* dbg, size_t dbg_sz) {
  char name_buf[256] = {0};
  int has_debuglink = 0;

  int fd = open(lib_path, O_RDONLY);
  if (fd >= 0) {
    Elf64_Ehdr ehdr;
    if (read_elf_header(fd, &ehdr)) {
      Elf64_Shdr shdr;
      if (find_section(fd, &ehdr, ".gnu_debuglink", &shdr) == 0) {
        char* data = malloc(shdr.sh_size);
        if (data) {
          lseek(fd, shdr.sh_offset, SEEK_SET);
          if (read(fd, data, shdr.sh_size) == (ssize_t)shdr.sh_size) {
            strncpy(name_buf, data, sizeof(name_buf) - 1);
            name_buf[sizeof(name_buf) - 1] = '\0';
            has_debuglink = 1;
          }
          free(data);
        }
      }
    }
    close(fd);
  }

  char* lib_copy = strdup(lib_path);
  char* lib_dir = dirname(lib_copy);

  if (has_debuglink && name_buf[0]) {
    snprintf(dbg, dbg_sz, "%s/%s", lib_dir, name_buf);
    if (access(dbg, R_OK) == 0) {
      free(lib_copy);
      return 0;
    }
  }
  if (has_debuglink && name_buf[0]) {
    snprintf(dbg, dbg_sz, "%s/.debug/%s", lib_dir, name_buf);
    if (access(dbg, R_OK) == 0) {
      free(lib_copy);
      return 0;
    }
  }
  if (lib_path[0] == '/') {
    snprintf(dbg, dbg_sz, "/usr/lib/debug%s.debug", lib_path);
    if (access(dbg, R_OK) == 0) {
      free(lib_copy);
      return 0;
    }
  }
  snprintf(dbg, dbg_sz, "%s.debug", lib_path);
  if (access(dbg, R_OK) == 0) {
    free(lib_copy);
    return 0;
  }

  free(lib_copy);
  return -1;
}

int debug_file_has_symtab(const char* path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) return 0;
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) {
    close(fd);
    return 0;
  }
  Elf64_Shdr shdr;
  int ret = (find_section(fd, &ehdr, ".symtab", &shdr) == 0);
  close(fd);
  return ret;
}

int vaddr_to_file_offset(int fd, unsigned long vaddr, unsigned long* file_off) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) return -1;

  Elf64_Phdr phdr;
  for (int i = 0; i < ehdr.e_phnum; i++) {
    lseek(fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
    if (read(fd, &phdr, sizeof(phdr)) != sizeof(phdr)) return -1;
    if (phdr.p_type != PT_LOAD) continue;
    if (vaddr >= phdr.p_vaddr && vaddr < phdr.p_vaddr + phdr.p_memsz) {
      *file_off = phdr.p_offset + (vaddr - phdr.p_vaddr);
      return 0;
    }
  }
  return -1;
}

int find_function_in_symtab(int fd, const char* prefix, unsigned long* out_off, size_t* out_sz) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) return -1;

  Elf64_Shdr symtab, strtab;
  if (find_section(fd, &ehdr, ".symtab", &symtab) != 0) return -1;
  if (find_section(fd, &ehdr, ".strtab", &strtab) != 0) return -1;

  char* strs = malloc(strtab.sh_size);
  if (!strs) return -1;
  lseek(fd, strtab.sh_offset, SEEK_SET);
  if (read(fd, strs, strtab.sh_size) != (ssize_t)strtab.sh_size) {
    free(strs);
    return -1;
  }

  size_t n_syms = symtab.sh_size / sizeof(Elf64_Sym);
  Elf64_Sym sym;
  int found = 0;

  for (size_t i = 0; i < n_syms; i++) {
    lseek(fd, symtab.sh_offset + i * sizeof(Elf64_Sym), SEEK_SET);
    if (read(fd, &sym, sizeof(sym)) != sizeof(sym)) break;

    if (ELF64_ST_TYPE(sym.st_info) != STT_FUNC) continue;
    if (sym.st_value == 0 || sym.st_size == 0) continue;

    const char* sname = strs + sym.st_name;
    if (strncmp(sname, prefix, strlen(prefix)) == 0) {
      if (!found || sym.st_size > *out_sz) {
        *out_off = sym.st_value;
        *out_sz = sym.st_size;
        found = 1;
      }
    }
  }

  free(strs);
  return found ? 0 : -1;
}

int get_build_id(int fd, unsigned char* id_out, size_t* id_len) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) return -1;

  Elf64_Shdr shdr;
  if (find_section(fd, &ehdr, ".note.gnu.build-id", &shdr) != 0)
    return -1;

  unsigned char* data = malloc(shdr.sh_size);
  if (!data || shdr.sh_size < 12) {
    free(data);
    return -1;
  }
  lseek(fd, shdr.sh_offset, SEEK_SET);
  if (read(fd, data, shdr.sh_size) != (ssize_t)shdr.sh_size) {
    free(data);
    return -1;
  }

  const unsigned char* p = data;
  const unsigned char* end = data + shdr.sh_size;

  while (p + 12 <= end) {
    uint32_t namesz = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    p += 4;
    uint32_t descsz = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    p += 4;
    uint32_t type = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    p += 4;

    size_t name_pad = (namesz + 3) & ~3;
    size_t desc_pad = (descsz + 3) & ~3;

    if (p + name_pad + desc_pad > end) break;

    if (type == 3 && namesz == 4 && memcmp(p, "GNU", 4) == 0) {
      size_t copy = descsz < *id_len ? descsz : *id_len;
      memcpy(id_out, p + name_pad, copy);
      *id_len = copy;
      free(data);
      return 0;
    }
    p += name_pad + desc_pad;
  }

  free(data);
  return -1;
}

int find_got_entry(int fd, const char* sym_name, unsigned long* got_vaddr) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) return -1;

  Elf64_Shdr rela_dyn, dynsym, dynstr;
  if (find_section(fd, &ehdr, ".rela.dyn", &rela_dyn) != 0) return -1;
  if (find_section(fd, &ehdr, ".dynsym", &dynsym) != 0) return -1;
  if (find_section(fd, &ehdr, ".dynstr", &dynstr) != 0) return -1;

  char* strs = malloc(dynstr.sh_size);
  if (!strs) return -1;
  lseek(fd, dynstr.sh_offset, SEEK_SET);
  if (read(fd, strs, dynstr.sh_size) != (ssize_t)dynstr.sh_size) {
    free(strs);
    return -1;
  }

  size_t n_syms = dynsym.sh_size / sizeof(Elf64_Sym);
  Elf64_Sym* syms = malloc(dynsym.sh_size);
  if (!syms) {
    free(strs);
    return -1;
  }
  lseek(fd, dynsym.sh_offset, SEEK_SET);
  if (read(fd, syms, dynsym.sh_size) != (ssize_t)dynsym.sh_size) {
    free(strs);
    free(syms);
    return -1;
  }

  size_t n_rela = rela_dyn.sh_size / sizeof(Elf64_Rela);
  Elf64_Rela* rela = malloc(rela_dyn.sh_size);
  if (!rela) {
    free(strs);
    free(syms);
    return -1;
  }
  lseek(fd, rela_dyn.sh_offset, SEEK_SET);
  if (read(fd, rela, rela_dyn.sh_size) != (ssize_t)rela_dyn.sh_size) {
    free(strs);
    free(syms);
    free(rela);
    return -1;
  }

  size_t namelen = strlen(sym_name);
  int found = -1;

  for (size_t i = 0; i < n_rela; i++) {
    unsigned long r_type = ELF64_R_TYPE(rela[i].r_info);
    if (r_type != 6) continue; /* R_X86_64_GLOB_DAT */

    unsigned long sym_idx = ELF64_R_SYM(rela[i].r_info);
    if (sym_idx >= n_syms) continue;

    const char* name = strs + syms[sym_idx].st_name;
    if (strncmp(name, sym_name, namelen) == 0 && name[namelen] == '\0') {
      *got_vaddr = rela[i].r_offset;
      found = 0;
      break;
    }
  }

  free(strs);
  free(syms);
  free(rela);
  return found;
}

int find_plt_entry(int fd, const char* sym_name, unsigned long* plt_vaddr) {
  Elf64_Ehdr ehdr;
  if (!read_elf_header(fd, &ehdr)) return -1;

  Elf64_Shdr rela_plt, dynsym, dynstr, plt;
  if (find_section(fd, &ehdr, ".rela.plt", &rela_plt) != 0) return -1;
  if (find_section(fd, &ehdr, ".dynsym", &dynsym) != 0) return -1;
  if (find_section(fd, &ehdr, ".dynstr", &dynstr) != 0) return -1;

  int is_plt_sec = (find_section(fd, &ehdr, ".plt.sec", &plt) == 0);
  if (!is_plt_sec && find_section(fd, &ehdr, ".plt", &plt) != 0)
    return -1;

  char* strs = malloc(dynstr.sh_size);
  if (!strs) return -1;
  lseek(fd, dynstr.sh_offset, SEEK_SET);
  if (read(fd, strs, dynstr.sh_size) != (ssize_t)dynstr.sh_size) {
    free(strs);
    return -1;
  }

  size_t n_syms = dynsym.sh_size / sizeof(Elf64_Sym);
  Elf64_Sym* syms = malloc(dynsym.sh_size);
  if (!syms) {
    free(strs);
    return -1;
  }
  lseek(fd, dynsym.sh_offset, SEEK_SET);
  if (read(fd, syms, dynsym.sh_size) != (ssize_t)dynsym.sh_size) {
    free(strs);
    free(syms);
    return -1;
  }

  size_t n_rela = rela_plt.sh_size / sizeof(Elf64_Rela);
  Elf64_Rela* rela = malloc(rela_plt.sh_size);
  if (!rela) {
    free(strs);
    free(syms);
    return -1;
  }
  lseek(fd, rela_plt.sh_offset, SEEK_SET);
  if (read(fd, rela, rela_plt.sh_size) != (ssize_t)rela_plt.sh_size) {
    free(strs);
    free(syms);
    free(rela);
    return -1;
  }

  size_t namelen = strlen(sym_name);
  int found = -1;

  for (size_t i = 0; i < n_rela; i++) {
    unsigned long sym_idx = ELF64_R_SYM(rela[i].r_info);
    if (sym_idx >= n_syms) continue;
    if (ELF64_ST_TYPE(syms[sym_idx].st_info) != STT_FUNC) continue;

    const char* name = strs + syms[sym_idx].st_name;
    if (strncmp(name, sym_name, namelen) == 0 && name[namelen] == '\0') {
      *plt_vaddr = plt.sh_addr + (is_plt_sec ? 0 : 16) + i * 16;
      found = 0;
      break;
    }
  }

  free(strs);
  free(syms);
  free(rela);
  return found;
}
