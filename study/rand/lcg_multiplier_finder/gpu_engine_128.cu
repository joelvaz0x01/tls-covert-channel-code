/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gpu_engine.h"
#include "settings.h"
#include "spectral_test.h"
#include "uint128_ops.cuh"

/**
 * Gets the n-th bit of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @param n The bit index (0-127).
 * @return The value of the n-th bit (0 or 1).
 */
static inline int get_bit128(uint128_t v, int n) {
  return (n >= 64) ? (int)((v.hi >> (n - 64)) & 1) : (int)((v.lo >> n) & 1);
}

/**
 * Sets the n-th bit of a 128-bit unsigned integer to 1.
 *
 * @param v Pointer to the 128-bit unsigned integer.
 * @param n The bit index (0-127).
 */
static inline void set_bit128(uint128_t* v, int n) {
  if (n >= 64)
    v->hi |= (1ULL << (n - 64));
  else
    v->lo |= (1ULL << n);
}

/**
 * Divides a 128-bit unsigned integer by 10 and returns the remainder.
 * using bit-by-bit long division.
 *
 * @param v Pointer to the 128-bit unsigned integer (updated to quotient).
 * @return The remainder (0-9).
 */
static inline unsigned int divmod10_128(uint128_t* v) {
  uint128_t q = {0, 0};
  unsigned int rem = 0;
  for (int i = 127; i >= 0; i--) {
    rem = (rem << 1) | get_bit128(*v, i);
    if (rem >= 10) {
      rem -= 10;
      set_bit128(&q, i);
    }
  }
  *v = q;
  return rem;
}

/**
 * Prints a 128-bit unsigned integer in decimal format.
 *
 * @param v The 128-bit unsigned integer to print.
 */
static void print128_dec(uint128_t v) {
  if (isZero128(v)) {
    printf("0");
    return;
  }
  char str[45];
  int pos = 44;
  str[pos] = '\0';
  uint128_t temp = v;
  while (!isZero128(temp)) {
    str[--pos] = (char)('0' + divmod10_128(&temp));
  }
  printf("%s", &str[pos]);
}

/**
 * Checks if a given 128-bit unsigned integer is an optimal exact multiplier.
 *
 * This uses the Euclidean algorithm to calculate the continued
 * fraction expansion of lambda to ensure good 2D spectral quality.
 *
 * @param lambda The 128-bit unsigned integer to check.
 * @return 1 if lambda is an optimal exact multiplier, 0 otherwise.
 */
static __device__ int is_optimal_exact_128(uint128_t lambda) {
  uint128_t den = lambda;
  uint128_t num = neg128(lambda);

  while (!isZero128(num)) {
    int q_count = 0;
    uint128_t rem = den;

    while (gte128(rem, num)) {
      rem = sub128(rem, num);
      q_count++;
      if (q_count > MAX_K_128) return 0;
    }
    den = num;
    num = rem;
  }
  return 1;
}

/**
 * Kernel function for searching for optimal exact multipliers on the GPU.
 *
 * @param start_lambda The starting 128-bit unsigned integer for the search.
 * @param d_candidates A device pointer to store the found candidates.
 * @param d_count A device pointer to store the count of found candidates.
 */
static __global__ void search_kernel_128(uint128_t start_lambda, uint128_t* d_candidates, int* d_count) {
  unsigned long long idx = blockIdx.x * blockDim.x + threadIdx.x;
  uint128_t offset;
  offset.hi = 0;
  offset.lo = idx * 8;

  uint128_t my_lambda = add128(start_lambda, offset);

  if (is_optimal_exact_128(my_lambda)) {
    int pos = atomicAdd(d_count, 1);
    if (pos < MAX_CANDIDATES_PER_BATCH) {
      d_candidates[pos] = my_lambda;
    }
  }
}

/**
 * Saves a checkpoint of the 128-bit search progress to a file.
 *
 * @param filename The name of the file to save the checkpoint to.
 * @param lambda The current 128-bit unsigned integer being searched.
 * @param batches The number of batches processed so far.
 * @param accum_time The accumulated time spent searching so far.
 */
static void save_checkpoint_128(const char* filename, uint128_t lambda, unsigned long long batches, double accum_time) {
  FILE* f = fopen(filename, "w");
  if (f) {
    fprintf(f, "%llx %llx %llu %f\n", (unsigned long long)lambda.hi, (unsigned long long)lambda.lo, batches, accum_time);
    fclose(f);
  }
}

/**
 * Loads a checkpoint of the 128-bit search progress from a file.
 *
 * @param filename The name of the file to load the checkpoint from.
 * @param lambda A pointer to store the loaded 128-bit unsigned integer.
 * @param batches A pointer to store the loaded number of batches.
 * @param accum_time A pointer to store the loaded accumulated time.
 * @return True if the checkpoint was successfully loaded, false otherwise.
 */
static bool load_checkpoint_128(const char* filename, uint128_t* lambda, unsigned long long* batches, double* accum_time) {
  FILE* f = fopen(filename, "r");
  if (f) {
    unsigned long long hi, lo;
    if (fscanf(f, "%llx %llx %llu %lf", &hi, &lo, batches, accum_time) == 4) {
      lambda->hi = hi;
      lambda->lo = lo;
      fclose(f);
      return true;
    }
    fclose(f);
  }
  return false;
}

void run_128_bit_search(void) {
  printf("--- 128-Bit Mode: GPU Fast-Filter & CPU Spectral Test ---\n\n");

  uint128_t start_lambda;
  unsigned long long batches = 0;
  double accumulated_time = 0.0;
  const char* checkpoint_file = "search_state_128.txt";

  if (load_checkpoint_128(checkpoint_file, &start_lambda, &batches, &accumulated_time)) {
    printf("Found checkpoint! Resuming search...\n");
    printf("Starting from: 0x%016llx%016llx\n", (unsigned long long)start_lambda.hi, (unsigned long long)start_lambda.lo);
    printf("Time previously spent: ");
    format_and_print_time(accumulated_time);
    printf("\n\n");
  } else {
    printf("No checkpoint found. Starting fresh from Golden Ratio...\n\n");
    start_lambda.hi = GOLDEN_RATIO_128_HI;
    start_lambda.lo = GOLDEN_RATIO_128_LO;
  }

  int h_count = 0;
  int* d_count;
  uint128_t* d_candidates;
  uint128_t h_candidates[MAX_CANDIDATES_PER_BATCH];

  cudaMalloc((void**)&d_count, sizeof(int));
  cudaMalloc((void**)&d_candidates, MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t));

  unsigned long long threads_per_batch = (unsigned long long)BLOCKS_PER_GRID * THREADS_PER_BLOCK;

  uint128_t lambda_stride_per_batch;
  lambda_stride_per_batch.hi = 0;
  lambda_stride_per_batch.lo = threads_per_batch * 8;

  time_t session_start_time = time(NULL);
  bool found = false;

  while (!found) {
    h_count = 0;
    cudaMemcpy(d_count, &h_count, sizeof(int), cudaMemcpyHostToDevice);

    search_kernel_128<<<BLOCKS_PER_GRID, THREADS_PER_BLOCK>>>(start_lambda, d_candidates, d_count);
    cudaDeviceSynchronize();

    cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

    if (h_count > 0) {
      int process_count = (h_count > MAX_CANDIDATES_PER_BATCH) ? MAX_CANDIDATES_PER_BATCH : h_count;
      cudaMemcpy(h_candidates, d_candidates, process_count * sizeof(uint128_t), cudaMemcpyDeviceToHost);

      for (int i = 0; i < process_count; i++) {
        if (passes_higher_dimensions_128(h_candidates[i])) {
          double total_time = accumulated_time + difftime(time(NULL), session_start_time);

          printf("\n==========================================\n");
          printf("SUCCESS! Found 128-bit Multiplier passing ALL dimensions!\n");
          printf("Lambda (Hex): 0x%016llx%016llx\n", (unsigned long long)h_candidates[i].hi, (unsigned long long)h_candidates[i].lo);
          printf("Lambda (Dec): ");
          print128_dec(h_candidates[i]);
          printf("\n\nTOTAL SEARCH TIME: ");
          format_and_print_time(total_time);
          printf("\n==========================================\n");

          found = true;
          remove(checkpoint_file);
          break;
        }
      }
    }

    if (found) break;

    start_lambda = add128(start_lambda, lambda_stride_per_batch);
    batches++;

    if (batches % CHECKPOINT_INTERVAL == 0) {
      double current_session_time = difftime(time(NULL), session_start_time);
      save_checkpoint_128(checkpoint_file, start_lambda, batches, accumulated_time + current_session_time);

      if (current_session_time > 0) {
        unsigned long long total_checked = batches * threads_per_batch;
        double speed_m_sec = (total_checked / 1000000.0) / current_session_time;
        printf("\rChecked %llu million candidates... (Speed: %.2f M/sec)   ", total_checked / 1000000, speed_m_sec);
        fflush(stdout);
      }
    }
  }

  cudaFree(d_candidates);
  cudaFree(d_count);
}
