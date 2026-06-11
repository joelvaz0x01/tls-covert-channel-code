/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbprng/cbprng.h>
#include <cbprng/settings.h>
#include <rand/rand.h>

typedef struct {
  int* S;  // S-boxes: [layer][sbox][index]
  int* P;  // P-boxes: [layer][bit]
  int bits;
  int layers;
  int sbox_bits;
} dynamic_gen_t;

mask_t dynamic_generate(dynamic_gen_t* g, mask_t counter) {
  mask_t bits = counter;
  mask_t sbox_mask = (1ULL << g->sbox_bits) - 1;
  int n_sboxes = g->bits / g->sbox_bits;

  for (int l = 0; l < g->layers; l++) {
    mask_t next = 0;
    // S-Box Layer
    for (int s = 0; s < n_sboxes; s++) {
      int input = (bits >> (s * g->sbox_bits)) & sbox_mask;
      int output = g->S[l * n_sboxes * (1 << g->sbox_bits) + s * (1 << g->sbox_bits) + input];
      next |= (mask_t)output << (s * g->sbox_bits);
    }

    if (l < g->layers - 1) {
      bits = 0;
      // P-Box Layer
      for (int b = 0; b < g->bits; b++) {
        int target_bit = g->P[l * g->bits + b];
        bits |= ((next >> b) & 1ULL) << target_bit;
      }
    } else {
      bits = next;
    }
  }
  return bits;
}

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

  g->S = malloc(g->layers * n_sboxes * sbox_size * sizeof(int));
  g->P = malloc((g->layers - 1) * bits * sizeof(int));

  // Fill with random permutations (simplified for test purposes)
  for (int l = 0; l < g->layers; l++) {
    for (int s = 0; s < n_sboxes; s++) {
      int* box = &g->S[l * n_sboxes * sbox_size + s * sbox_size];
      for (int i = 0; i < sbox_size; i++) box[i] = i;
      for (int i = sbox_size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = box[i];
        box[i] = box[j];
        box[j] = tmp;
      }
    }
  }
  for (int l = 0; l < g->layers - 1; l++) {
    int* box = &g->P[l * bits];
    for (int i = 0; i < bits; i++) box[i] = i;
    for (int i = bits - 1; i > 0; i--) {
      int j = rand() % (i + 1);
      int tmp = box[i];
      box[i] = box[j];
      box[j] = tmp;
    }
  }
}

int test_repetition(int bits) {
  uint64_t total = 1ULL << bits;
  char buffer[128];

  snprintf(buffer, sizeof(buffer), "Finding duplicates in %2d-bit CBPRNG (%llu values)...", bits, (unsigned long long)total);
  printf("%-57s", buffer);
  fflush(stdout);

  dynamic_gen_t g;
  init_dynamic_gen(&g, bits);

  uint8_t* seen = calloc(total, 1);
  if (!seen) {
    printf(" [Memory Fail]\n");
    free_dynamic_gen(&g);
    return 0;
  }

  for (uint64_t i = 0; i < total; i++) {
    mask_t val = dynamic_generate(&g, (mask_t)i);
    if (val >= total || seen[val]) {
      printf("[FAIL] Duplicated value or out of range!\n");
      free(seen);
      free_dynamic_gen(&g);
      return 0;
    }
    seen[val] = 1;
  }

  printf("[OK]\n");
  free(seen);
  free_dynamic_gen(&g);
  return 1;
}

int main(void) {
  seed_prng();
  printf("\nCBPRNG Exhaustive Duplicate Finder\n");
  printf("---------------------------------------------------------------\n");

  int max_test_bits = (CBPRNG_BITS > 24) ? 24 : CBPRNG_BITS;
  int b;

  for (b = S_BOX_BITS; b <= max_test_bits; b += S_BOX_BITS) {
    if (!test_repetition(b)) return 1;
  }

  printf("\nNo repetitions found from %d-bit to %d-bit permutations.\n", S_BOX_BITS, max_test_bits);

  if (CBPRNG_BITS > 24) {
    printf("[INFO] Tests from %d-bit till %d-bit CBPRNG skipped.\n", b, CBPRNG_BITS);
  }

  printf("---------------------------------------------------------------\n");
  printf("All tests passed successfully!\n\n");
  return 0;
}
