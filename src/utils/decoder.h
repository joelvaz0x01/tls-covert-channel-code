/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UTILS_DECODER_H
#define UTILS_DECODER_H

#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

/**
 * Checks if the data in the field is valid.
 *
 * @param field The field to check.
 * @return true if the data is valid, false otherwise.
 */
bool is_data_valid(const tls_mod_rand_t* field);

/**
 * Calculates the file ID from the field.
 *
 * @param field The field to calculate the file ID from.
 * @param id The calculated file ID.
 * @return true if the file ID was calculated successfully, false otherwise.
 */
bool calculate_file_id(const tls_mod_rand_t* field, uint64_t id);

/**
 * Returns the number of bytes to write for a given file part.
 *
 * @param file_part The file part index.
 * @param n The total number of file parts.
 * @return The number of bytes to write.
 */
size_t bytes_to_write(uint64_t file_part, uint64_t n);

#endif /* UTILS_DECODER_H */
