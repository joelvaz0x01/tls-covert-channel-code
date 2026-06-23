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
 * Saves the resultant Fountain Code into a file in binary mode.
 *
 * @param filename The name of the file to save.
 * @param mod_rand The modified TLS random field.
 * @return 0 on success, -1 on error.
 */
int save_fountain_code(const char* filename, const tls_mod_rand_t mod_rand);

#endif /* UTILS_FILE_H */
