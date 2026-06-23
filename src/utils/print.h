/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef UTILS_PRINT_H
#define UTILS_PRINT_H

/**
 * Prints the given data as a hex string for a specific number of bits.
 *
 * @param d Pointer to the data.
 * @param bits Number of bits to print.
 */
void print_hex_bits(const void* d, int bits);

/**
 * Prints the given data as ASCII for a specific number of bits.
 *
 * @param d Pointer to the data.
 * @param bits Number of bits to print.
 */
void print_ascii_bits(const void* d, int bits);

#endif /* UTILS_PRINT_H */
