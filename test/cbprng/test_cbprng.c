/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>
#include <cbprng/p_box.h>
#include <cbprng/s_box.h>
#include <cbprng/settings.h>

#include <rand/system.h>

#define MAX_TEST_BITS 24

typedef struct {
  permutation_t* S;
  permutation_t* P;
  int bits;
  int layers;
  int sbox_bits;
} dynamic_gen_t;

void free_dynamic_gen(dynamic_gen_t* g) {
  free(g->S);
  free(g->P);
}

void init_dynamic_gen(dynamic_gen_t* g, int bits) {
  g->bits = bits;
  g->layers = N_LAYERS;
  g->sbox_bits = S_BOX_BITS;
  int sbox_size = 1 << g->sbox_bits;
  int n_sboxes = bits / g->sbox_bits;

  g->S = malloc(g->layers * n_sboxes * sbox_size * sizeof(permutation_t));
  g->P = malloc((g->layers - 1) * bits * sizeof(permutation_t));

  /* fill S-Box with random permutations */
  for (int l = 0; l < g->layers; l++) {
    for (int s = 0; s < n_sboxes; s++) {
      permutation_t* box = &g->S[l * n_sboxes * sbox_size + s * sbox_size];
      pseudo_random_s_box((s_box_t*)box);
    }
  }

  /* fill P-Box with random permutations */
  for (int l = 0; l < g->layers - 1; l++) {
    permutation_t* box = &g->P[l * bits];
    pseudo_random_permutation(bits, box);
  }
}

bool test_repetition(int bits) {
  uint64_t total = 1ULL << bits;
  char buffer[128];

  seed64_system();

  snprintf(buffer, sizeof(buffer), "Finding duplicates in %2d-bit CBPRNG (%llu values)...", bits, (unsigned long long)total);
  printf("%-57s", buffer);
  fflush(stdout);

  dynamic_gen_t g;
  init_dynamic_gen(&g, bits);

  uint8_t* seen = calloc(total, 1);
  if (!seen) {
    printf(" [Memory Fail]\n\n");
    free_dynamic_gen(&g);
    return false;
  }

  uint64_t step = total / 100;
  if (step == 0) step = 1;

  for (uint64_t i = 0; i < total; i++) {
    if (i % step == 0) {
      printf("\r%-59s [%3" PRIu64 "%%]", buffer, (i * 100) / total);
      fflush(stdout);
    }
    mask_t val = generate_cbprng_generic(g.S, g.P, (mask_t)i, g.bits, g.layers, g.sbox_bits);
    if (val >= total) {
      printf("\r%-59s [FAIL] Out of range!\n\n", buffer);
      free(seen);
      free_dynamic_gen(&g);
      return false;
    }
    if (seen[val]) {
      printf("\r%-59s [FAIL] Duplicated value!\n\n", buffer);
      free(seen);
      free_dynamic_gen(&g);
      return false;
    }
    seen[val] = 1;
  }

  printf("\r%-59s [OK]     \n", buffer);
  free(seen);
  free_dynamic_gen(&g);
  return true;
}

int main(void) {
  printf("\nCBPRNG Exhaustive Duplicate Finder\n");
  printf("-------------------------------------------------------------------------------------\n");

  int max_test_bits = (CBPRNG_BITS > MAX_TEST_BITS) ? MAX_TEST_BITS : CBPRNG_BITS;
  int b;

  for (b = S_BOX_BITS; b <= max_test_bits; b += S_BOX_BITS) {
    if (!test_repetition(b)) {
      printf("-------------------------------------------------------------------------------------\n");
      printf("Some repetition found at %d-bit CBPRNG\n\n", b);
      return 1;
    }
  }

  printf("\nNo repetitions found from %d-bit to %d-bit permutations.\n", S_BOX_BITS, max_test_bits);

  if (CBPRNG_BITS > MAX_TEST_BITS) {
    printf("[INFO] Tests from %d-bit till %d-bit CBPRNG skipped.\n", b, CBPRNG_BITS);
  }

  printf("-------------------------------------------------------------------------------------\n");
  printf("All tests passed successfully!\n\n");
  return 0;
}
