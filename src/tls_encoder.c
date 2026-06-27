#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>

#include <fountain_code/settings.h>
#include <fountain_code/utils.h>
#include <fountain_code/vec_ops.h>

#include <utils/encoder.h>
#include <utils/file.h>
#include <utils/utils.h>

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
    close_files();
    exit(EXIT_FAILURE);
  }

  const uint64_t n = calculate_n(src_file, true);
  if (n == 0 || n > MAX_BLOCKS) {
    fprintf(stderr, "[-] message size exceeds MAX_BLOCKS: expected <= %d, got %lu\n", MAX_BLOCKS, n);
    exit(EXIT_FAILURE);
  }

  const uint64_t m = generate_m(n);
  const uint64_t n_words = init_program(n, m);

  init_cbprng();

  while (0 != dec->remaining) {
    uint64_t seed = generate_cbprng(&cbprng, counter_value);
    tls_mod_rand_t data = modified_random_field(0, &seed, m, n, n_words, src_file);

    if (0 != save_encoder(dest_file, data)) {
      fprintf(stderr, "\n[-] failed to write block %lu\n", counter_value);
      close_files();
      finalize_program();
      exit(EXIT_FAILURE);
    }

    counter_value++;
    fprintf(stderr, "[*] encoding: %lu/%lu\r", n - dec->remaining, n);
  }
  fprintf(stderr, "\n[!] Encoder finished successfully!\n");

  close_files();
  finalize_program();

  return 0;
}
