/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UTILS_FILE_H
#define UTILS_FILE_H

#include <stdint.h>

#include <fountain_code/utils.h>

#include "utils.h"

/**
 * Opens a file in binary mode for reading.
 *
 * @param filename The name of the file to open.
 * @param is_src Whether the file is a source file (1) or a destination file (0).
 * @return 1 on success, 0 on error.
 */
uint64_t open_file(const char* filename, const int is_src);

/**
 * Closes the file if it is open.
 */
void close_files(void);

/**
 * Calculates the number of file parts based on the file size.
 *
 * @param filename The name of the file.
 * @return The number of file parts.
 */
uint64_t calculate_n(const char* filename);

/**
 * Reads a block from a file in binary mode.
 *
 * @param filename The name of the file to read.
 * @param file_part The part of the file to read (0-based index).
 * @param buffer The buffer to read into.
 * @return 0 on success, -1 on error.
 */
int read_file_part(const char* filename, const uint64_t file_part, block_t* buffer);

/**
 * Returns the size of the source file in bytes.
 *
 * @return The source file size, or 0 if not yet opened.
 */
long get_src_size(void);

/**
 * Saves the encoder state (tls_mod_rand_t) in binary mode.
 *
 * @param filename The name of the file to save.
 * @param data The data to write.
 * @return 0 on success, -1 on error.
 */
int save_encoder(const char* filename, const tls_mod_rand_t data);

/**
 * Saves the decoder state (block_t) in binary mode.
 *
 * @param filename The name of the output file.
 * @param data The data to write.
 * @param n_bytes The number of bytes to write.
 * @return 0 on success, -1 on error.
 */
int save_decoder(const char* filename, const block_t data, size_t n_bytes);

#endif /* UTILS_FILE_H */
