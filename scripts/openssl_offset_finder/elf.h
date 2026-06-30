/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of parsing ELF
 */

#ifndef OPENSSL_OFFSET_ELF_H
#define OPENSSL_OFFSET_ELF_H

#include <elf.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Reads and validates the ELF header from an open file descriptor.
 *
 * Checks the ELF magic number and reads the full Elf64_Ehdr.
 *
 * @param fd Open file descriptor positioned at the start of the file.
 * @param ehdr Output buffer for the ELF header.
 * @return 1 on success, 0 on failure.
 */
int read_elf_header(int fd, Elf64_Ehdr* ehdr);

/**
 * Finds an ELF section by name.
 *
 * Scans the section header table for a section matching @p name.
 *
 * @param fd Open file descriptor.
 * @param ehdr Previously read ELF header.
 * @param name Section name to search for.
 * @param out Output buffer for the section header.
 * @return 0 on success, -1 if the section was not found or an error occurred.
 */
int find_section(int fd, const Elf64_Ehdr* ehdr, const char* name, Elf64_Shdr* out);

/**
 * Locates the debug symbol file for a given library path.
 *
 * Tries .gnu_debuglink, standard .debug directories, and
 * /usr/lib/debug/ paths to find matching debug symbols.
 *
 * @param lib_path Path to the library.
 * @param dbg Output buffer for the debug file path.
 * @param dbg_sz Size of the output buffer.
 * @return 0 if a debug file was found, -1 otherwise.
 */
int find_debug_path(const char* lib_path, char* dbg, size_t dbg_sz);

/**
 * Checks whether an ELF file contains a .symtab section.
 *
 * @param path Path to the ELF file.
 * @return 1 if .symtab exists, 0 otherwise.
 */
int debug_file_has_symtab(const char* path);

/**
 * Converts a virtual address to a file offset using the program headers.
 *
 * Iterates the PT_LOAD segments to find the one containing @p vaddr.
 *
 * @param fd Open file descriptor.
 * @param vaddr Virtual address to convert.
 * @param file_off Output for the corresponding file offset.
 * @return 0 on success, -1 on failure.
 */
int vaddr_to_file_offset(int fd, unsigned long vaddr, unsigned long* file_off);

/**
 * Finds a function in the .symtab section by name prefix.
 *
 * Searches for symbols with type STT_FUNC whose name starts with @p prefix.
 *
 * @param fd Open file descriptor (ELF with .symtab).
 * @param prefix Function name prefix to match.
 * @param out_off Output for the function's virtual address.
 * @param out_sz Output for the function's size in bytes.
 * @return 0 on success, -1 on failure.
 */
int find_function_in_symtab(int fd, const char* prefix, unsigned long* out_off, size_t* out_sz);

/**
 * Reads the GNU build-ID from .note.gnu.build-id.
 *
 * @param fd Open file descriptor.
 * @param id_out Output buffer for the build-ID bytes.
 * @param id_len Input: capacity of id_out. Output: actual ID length.
 * @return 0 on success, -1 if no build-ID was found.
 */
int get_build_id(int fd, unsigned char* id_out, size_t* id_len);

/**
 * Locates the GOT entry address for a given symbol in .rela.dyn
 * (R_X86_64_GLOB_DAT).
 *
 * @param fd Open file descriptor to the ELF file.
 * @param sym_name Symbol name to search for.
 * @param got_vaddr Output for the GOT entry virtual address.
 * @return 0 on success, -1 on failure.
 */
int find_got_entry(int fd, const char* sym_name, unsigned long* got_vaddr);

/**
 * Locates the PLT entry address for a given symbol in .rela.plt.
 *
 * @param fd Open file descriptor to the ELF file.
 * @param sym_name Symbol name to search for.
 * @param plt_vaddr Output for the PLT entry virtual address.
 * @return 0 on success, -1 on failure.
 */
int find_plt_entry(int fd, const char* sym_name, unsigned long* plt_vaddr);

#endif /* OPENSSL_OFFSET_ELF_H */
