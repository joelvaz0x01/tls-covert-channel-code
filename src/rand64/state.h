/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#ifndef RAND64_SETTINGS_H
#define RAND64_SETTINGS_H

#include <stdint.h>

/* degree of the trinomial polynomial - number of 64-bit words in the state table. */
#define RAND64_DEG 31

/* trinomial separation */
#define PRNG_SEP   3

/* PCG multiplier values based on musl libc 64-bit LCG */
#define INIT_MUL   UINT64_C(6364136223846793005)
#define INIT_ADD   UINT64_C(1)

extern uint64_t prng_state[RAND64_DEG]; /* state table         */
extern uint64_t* prng_fptr;             /* front pointer       */
extern uint64_t* prng_rptr;             /* rear pointer        */
extern int prng_initialized;            /* is PRNG initialized */
extern uint64_t prng_seed;              /* last seed           */

#endif /* RAND64_SETTINGS_H */
