/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <time.h>

#include "gpu_config.h"
#include "gpu_engine.h"
#include "settings.h"
#include "spectral_test.h"

/**
 * Checks if a given lambda value is optimal for 64-bit LCG.
 *
 * This uses the Euclidean algorithm to calculate the continued
 * fraction expansion of lambda to ensure good 2D spectral quality.
 *
 * Based on the article "Optimal multipliers for pseudo-random number
 * generation by the linear congruential method" by Borosh and Niederreiter
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
static __global__ __launch_bounds__(256, 2) void search_kernel_64(unsigned long long start_lambda, unsigned long long* d_candidates, int* d_count) {
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

  GpuConfig cfg = optimize_launch_config(search_kernel_64);
  print_gpu_info(cfg);

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

  int* d_count[2];
  unsigned long long* d_candidates[2];
  cudaStream_t streams[2];

  for (int i = 0; i < 2; i++) {
    cudaMalloc((void**)&d_count[i], sizeof(int));
    cudaMalloc((void**)&d_candidates[i], MAX_CANDIDATES_PER_BATCH * sizeof(unsigned long long));
    cudaStreamCreate(&streams[i]);
  }

  int* h_count[2];
  unsigned long long* h_candidates[2];
  for (int i = 0; i < 2; i++) {
    cudaMallocHost((void**)&h_count[i], sizeof(int));
    cudaMallocHost((void**)&h_candidates[i], MAX_CANDIDATES_PER_BATCH * sizeof(unsigned long long));
    h_count[i][0] = 0;
  }

  unsigned long long lambda_stride_per_batch = compute_batch_stride(cfg);
  int cur = 0;

  time_t session_start_time = time(NULL);
  time_t last_checkpoint_time = session_start_time;
  unsigned long long last_batches = batches;
  bool found = false;

  cudaMemsetAsync(d_count[cur], 0, sizeof(int), streams[cur]);
  search_kernel_64<<<cfg.grid_size, cfg.block_size, 0, streams[cur]>>>(start_lambda, d_candidates[cur], d_count[cur]);
  cudaMemcpyAsync(h_count[cur], d_count[cur], sizeof(int), cudaMemcpyDeviceToHost, streams[cur]);
  cudaMemcpyAsync(h_candidates[cur], d_candidates[cur], MAX_CANDIDATES_PER_BATCH * sizeof(unsigned long long), cudaMemcpyDeviceToHost, streams[cur]);
  cudaStreamSynchronize(streams[cur]);

  while (!found) {
    int next = 1 - cur;
    unsigned long long next_lambda = start_lambda + lambda_stride_per_batch;

    cudaMemsetAsync(d_count[next], 0, sizeof(int), streams[next]);
    search_kernel_64<<<cfg.grid_size, cfg.block_size, 0, streams[next]>>>(next_lambda, d_candidates[next], d_count[next]);

    cudaMemcpyAsync(h_count[cur], d_count[cur], sizeof(int), cudaMemcpyDeviceToHost, streams[cur]);
    cudaMemcpyAsync(h_candidates[cur], d_candidates[cur], MAX_CANDIDATES_PER_BATCH * sizeof(unsigned long long), cudaMemcpyDeviceToHost, streams[cur]);
    cudaStreamSynchronize(streams[cur]);

    if (*h_count[cur] > 0) {
      int process_count = (*h_count[cur] > MAX_CANDIDATES_PER_BATCH) ? MAX_CANDIDATES_PER_BATCH : *h_count[cur];
      for (int i = 0; i < process_count; i++) {
        if (passes_higher_dimensions_64(h_candidates[cur][i])) {
          double total_time = accumulated_time + difftime(time(NULL), session_start_time);

          printf("\n==========================================\n");
          printf("SUCCESS! Found 64-bit Multiplier passing ALL dimensions!\n");
          printf("Lambda (Hex): 0x%016llx\n", h_candidates[cur][i]);
          printf("Lambda (Dec): %llu\n", h_candidates[cur][i]);
          printf("\nTIME TO FIND: ");
          format_and_print_time(total_time);
          printf("\n==========================================\n");

          found = true;
          remove(checkpoint_file);
          break;
        }
      }
    }

    cur = next;
    start_lambda = next_lambda;
    batches++;

    if (!found && batches % CHECKPOINT_INTERVAL == 0) {
      double current_session_time = difftime(time(NULL), session_start_time);
      save_checkpoint_64(checkpoint_file, start_lambda, batches, accumulated_time + current_session_time);

      double interval_time = difftime(time(NULL), last_checkpoint_time);
      if (interval_time > 0) {
        unsigned long long interval_checked = (batches - last_batches) * cfg.threads_per_batch;
        unsigned long long total_checked = batches * cfg.threads_per_batch;
        double speed_m_sec = (interval_checked / 1000000.0) / interval_time;
        printf("\rChecked %llu million candidates... (Speed: %.2f M/sec)   ", total_checked / 1000000, speed_m_sec);
        fflush(stdout);
      }
      last_batches = batches;
      last_checkpoint_time = time(NULL);
    }
  }

  cudaDeviceSynchronize();
  for (int i = 0; i < 2; i++) {
    cudaFreeHost(h_count[i]);
    cudaFreeHost(h_candidates[i]);
    cudaFree(d_candidates[i]);
    cudaFree(d_count[i]);
    cudaStreamDestroy(streams[i]);
  }
}
