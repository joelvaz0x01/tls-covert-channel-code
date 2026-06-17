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

/**
 * Checks if a given lambda value is optimal for 64-bit LCG.
 *
 * This uses the Euclidean algorithm to calculate the continued
 * fraction expansion of lambda to ensure good 2D spectral quality.
 *
 * @param lambda The lambda value to check.
 * @return 1 if optimal, 0 otherwise.
 */
static __device__ int is_optimal_exact_64(unsigned long long lambda) {
  unsigned long long den = lambda;
  unsigned long long num = ~lambda + 1;

  while (num != 0) {
    int q_count = 0;
    unsigned long long rem = den;

    while (rem >= num) {
      rem -= num;
      q_count++;
      if (q_count > MAX_K_64) return 0;
    }
    den = num;
    num = rem;
  }
  return 1;
}

/**
 * Searches for optimal lambda values for 64-bit LCG on the GPU.
 *
 * @param start_lambda The starting lambda value to search from.
 * @param d_candidates Device pointer to store the found candidate lambdas.
 * @param d_count Device pointer to store the count of found candidates.
 */
static __global__ void search_kernel_64(unsigned long long start_lambda, unsigned long long* d_candidates, int* d_count) {
  unsigned long long idx = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned long long my_lambda = start_lambda + (idx * 8);

  if (is_optimal_exact_64(my_lambda)) {
    int pos = atomicAdd(d_count, 1);
    if (pos < MAX_CANDIDATES_PER_BATCH) {
      d_candidates[pos] = my_lambda;
    }
  }
}

/**
 * Saves a checkpoint of the 64-bit LCG search results to a file.
 *
 * @param filename The name of the file to save the checkpoint to.
 * @param lambda The current lambda value.
 * @param batches The number of batches processed.
 * @param accum_time The accumulated time spent searching.
 */
static void save_checkpoint_64(const char* filename, unsigned long long lambda, unsigned long long batches, double accum_time) {
  FILE* f = fopen(filename, "w");
  if (f) {
    fprintf(f, "%llx %llu %f\n", lambda, batches, accum_time);
    fclose(f);
  }
}

/**
 * Loads a checkpoint of the 64-bit LCG search results from a file.
 *
 * @param filename The name of the file to load the checkpoint from.
 * @param lambda Pointer to store the loaded lambda value.
 * @param batches Pointer to store the loaded number of batches.
 * @param accum_time Pointer to store the loaded accumulated time.
 * @return true if the checkpoint was successfully loaded, false otherwise.
 */
static bool load_checkpoint_64(const char* filename, unsigned long long* lambda, unsigned long long* batches, double* accum_time) {
  FILE* f = fopen(filename, "r");
  if (f) {
    if (fscanf(f, "%llx %llu %lf", lambda, batches, accum_time) == 3) {
      fclose(f);
      return true;
    }
    fclose(f);
  }
  return false;
}

void run_64_bit_search(void) {
  printf("--- 64-Bit Mode: GPU Fast-Filter & CPU Spectral Test ---\n\n");

  unsigned long long start_lambda;
  unsigned long long batches = 0;
  double accumulated_time = 0.0;
  const char* checkpoint_file = "search_state_64.txt";

  if (load_checkpoint_64(checkpoint_file, &start_lambda, &batches, &accumulated_time)) {
    printf("Found checkpoint! Resuming search...\n");
    printf("Starting from: 0x%016llx\n", start_lambda);
    printf("Time previously spent: ");
    format_and_print_time(accumulated_time);
    printf("\n\n");
  } else {
    printf("No checkpoint found. Starting fresh from Golden Ratio...\n\n");
    start_lambda = GOLDEN_RATIO_64;
  }

  int h_count = 0;
  int* d_count;
  unsigned long long* d_candidates;
  unsigned long long h_candidates[MAX_CANDIDATES_PER_BATCH];

  cudaMalloc((void**)&d_count, sizeof(int));
  cudaMalloc((void**)&d_candidates, MAX_CANDIDATES_PER_BATCH * sizeof(unsigned long long));

  unsigned long long threads_per_batch = (unsigned long long)BLOCKS_PER_GRID * THREADS_PER_BLOCK;
  unsigned long long lambda_stride_per_batch = threads_per_batch * 8;

  time_t session_start_time = time(NULL);
  bool found = false;

  while (!found) {
    h_count = 0;
    cudaMemcpy(d_count, &h_count, sizeof(int), cudaMemcpyHostToDevice);

    search_kernel_64<<<BLOCKS_PER_GRID, THREADS_PER_BLOCK>>>(start_lambda, d_candidates, d_count);
    cudaDeviceSynchronize();

    cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

    if (h_count > 0) {
      int process_count = (h_count > MAX_CANDIDATES_PER_BATCH) ? MAX_CANDIDATES_PER_BATCH : h_count;
      cudaMemcpy(h_candidates, d_candidates, process_count * sizeof(unsigned long long), cudaMemcpyDeviceToHost);

      for (int i = 0; i < process_count; i++) {
        if (passes_higher_dimensions_64(h_candidates[i])) {
          double total_time = accumulated_time + difftime(time(NULL), session_start_time);

          printf("\n==========================================\n");
          printf("SUCCESS! Found 64-bit Multiplier passing ALL dimensions!\n");
          printf("Lambda (Hex): 0x%016llx\n", h_candidates[i]);
          printf("Lambda (Dec): %llu\n", h_candidates[i]);
          printf("\nTIME TO FIND: ");
          format_and_print_time(total_time);
          printf("\n==========================================\n");

          found = true;
          remove(checkpoint_file);
          break;
        }
      }
    }

    if (found) break;

    start_lambda += lambda_stride_per_batch;
    batches++;

    if (batches % CHECKPOINT_INTERVAL == 0) {
      double current_session_time = difftime(time(NULL), session_start_time);
      save_checkpoint_64(checkpoint_file, start_lambda, batches, accumulated_time + current_session_time);

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
