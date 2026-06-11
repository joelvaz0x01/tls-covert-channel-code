/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include "cbprng.h"
#include "p_box.h"

void pseudo_random_p_box(p_box_t* p) {
  pseudo_random_permutation(CBPRNG_BITS, p->a);
}
