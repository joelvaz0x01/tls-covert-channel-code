/**
 * @file print_byte_arrays.h
 * @brief Header file for utility functions for printing byte arrays.
 */

#ifndef PRINT_BYTE_ARRAYS_H
#define PRINT_BYTE_ARRAYS_H

/**
 * Prints the given byte array as hexadecimal.
 *
 * @param d Pointer to the byte array to print.
 * @param len Length of the byte array.
 */
void print_hex(const unsigned char *d, int len);

/**
 * Prints up to len bytes as printable ASCII (replace controls with '.').
 *
 * @param d Pointer to the byte array to print.
 * @param len Length of the byte array.
 */
void print_ascii(const unsigned char *d, int len);

#endif /* PRINT_BYTE_ARRAYS_H */
