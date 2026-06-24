/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_DECODER_H
#define FOUNTAIN_CODE_DECODER_H

#include "utils.h"

/**
 * Decoder: solve the decoder by back-substitution.
 *
 * This function reduces each pivot row so that it has exactly one
 * bit set (at its own pivot index), then copies the decoded blocks out.
 *
 * Scans i from n-1 down to 0, reducing each pivot row as it goes.
 *
 * @param dec Pointer to the decoder structure.
 * @param out_blocks Pointer to the output buffer where decoded blocks will be copied.
 */
void decoder_solve(decoder_t* dec, block_t* out_blocks);

#endif /* FOUNTAIN_CODE_DECODER_H */
