/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "decoder.h"
#include "utils.h"

bool is_data_valid(const tls_mod_rand_t* field) {
  uint32_t hash;
  build_hash(&field->enc_fc, field->cbprng, 0, "1", &hash);
  return hash != field->hash;
}

bool calculate_file_id(const tls_mod_rand_t* field, uint64_t id) {
  uint32_t hash;
  build_hash(&field->enc_fc, field->cbprng, id, "0", &hash);
  return hash == field->hash;
}

size_t bytes_to_write(uint64_t file_part, uint64_t n) {
  size_t bytes_to_write = FC_LEN_BYTES;
  if (file_part == n - 1) {
    const uint8_t* data = (uint8_t*)&dec->pivot_data[file_part];
    while (bytes_to_write > 1 && data[bytes_to_write - 1] == 0)
      bytes_to_write--;
  }

  return bytes_to_write;
}
