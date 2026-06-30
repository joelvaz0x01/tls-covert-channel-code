/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef FOUNTAIN_CODE_DECODER_H
#define FOUNTAIN_CODE_DECODER_H

#include "utils.h"

/**
 * Solves the fountain-code system by back-substitution.
 *
 * Reduces each pivot row so that it contains exactly one set bit
 * (at its own pivot index), then copies the decoded source blocks
 * into the caller-supplied buffer.
 *
 * Scans pivot indices from n-1 down to 0 so that higher-index
 * pivots are eliminated from lower-index rows.
 *
 * @param dec Pointer to the decoder state.
 * @param out_blocks Output buffer of n blocks; missing blocks are zeroed.
 */
void decoder_solve(decoder_t* dec, block_t* out_blocks);

#endif /* FOUNTAIN_CODE_DECODER_H */
