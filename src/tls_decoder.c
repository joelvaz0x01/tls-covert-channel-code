/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fountain_code/decode.h>
#include <fountain_code/encoder.h>
#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include <utils/decoder.h>
#include <utils/file.h>
#include <utils/utils.h>

static bool success = true;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "[-] usage: %s <input> <output>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char* src_file = argv[1];
  if (0 == open_file(src_file, 1)) {
    fprintf(stderr, "[-] could not open %s\n", src_file);
    exit(EXIT_FAILURE);
  }

  const char* dest_file = argv[2];
  if (0 == open_file(dest_file, 0)) {
    fprintf(stderr, "[-] could not open %s\n", dest_file);
    goto cleanup;
  }

  const uint64_t n = calculate_n(src_file, false);
  if (n == 0 || n > MAX_BLOCKS) {
    fprintf(stderr, "[-] invalid number of source blocks: %lu (max %d)\n", n, MAX_BLOCKS);
    close_files();
    exit(EXIT_FAILURE);
  }

  const uint64_t m = generate_m(n);
  const uint64_t n_words = init_program(n, m);

  int packets_seen = 0;
  while (0 != dec->remaining) {
    tls_mod_rand_t rec;
    if (0 != read_tls_data(&rec)) break; /* EOF or error */
    if (!is_data_valid(&rec)) continue;  /* not a Fountain Code packet */

    uint64_t file_id = 0;
    bool found = false;
    for (uint64_t c = 0; c < MAX_BLOCKS; c++) {
      if (calculate_file_id(&rec, c)) {
        file_id = c;
        found = true;
        break;
      }
    }
    if (!found) {
      fprintf(stderr, "[-] malformed file: no matching file_id\n");
      success = false;
      goto cleanup;
    }

    cypher_fountain(&rec.enc_fc, rec.cbprng, file_id);
    construct_k(rec.cbprng, m, n, n_words, NULL);

    g_scratch_pkt->id = packets_seen++;
    g_scratch_pkt->data = rec.enc_fc;

    decoder_feed(dec, g_scratch_pkt);
  }

  if (0 != dec->remaining) {
    fprintf(stderr, "[-] not enough packets to reconstruct (remaining=%lu)\n", dec->remaining);
    success = false;
    goto cleanup;
  }

  decoder_solve(dec, NULL);

  for (uint64_t i = 0; i < n; i++) {
    size_t to_write = bytes_to_write(i, n);
    if (0 != save_decoder(dest_file, dec->pivot_data[i], to_write)) {
      fprintf(stderr, "[-] failed to write block %lu\n", i);
      success = false;
      goto cleanup;
    }
  }

cleanup:
  close_files();
  finalize_program();

  return success ? 0 : 1;
}
