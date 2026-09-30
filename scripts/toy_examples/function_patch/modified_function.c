/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * Toy example adapted from Tomás Oliveira e Silva, May 2026
 */

#include "victim_program.h"

void modified_function(void* addr, int count) {
  long* local_data = (long*)0x1122334455667788;
  long i = local_data[0];
  // local_data[0] = current record number (4096/32 = 128 records, record 0 is not used)
  if (count == 32 && i < 128L) {
    local_data[0] = i + 1L;
    local_data += 4L * i;
    ((long*)addr)[0] = local_data[0];
    ((long*)addr)[1] = local_data[1];
    ((long*)addr)[2] = local_data[2];
    ((long*)addr)[3] = local_data[3];
  } else
    original_function(addr, count);
}
