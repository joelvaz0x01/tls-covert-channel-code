/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gpu_config.h"
#include "gpu_engine.h"
#include "settings.h"
#include "spectral_test.h"
#include "uint128_ops.cuh"

#if LCG_UINT128_HAS_NATIVE

/**
 * Gets the n-th bit of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @param n The bit index (0-127).
 * @return The value of the n-th bit (0 or 1).
 */
static inline uint64_t get_lo128(uint128_t v) {
  return (uint64_t)v;
}

/**
 * Gets the high 64 bits of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @return The high 64 bits of the integer.
 */
static inline uint64_t get_hi128(uint128_t v) {
  return (uint64_t)(v >> 64);
}

/**
 * Constructs a 128-bit unsigned integer from a high and low 64-bit values.
 *
 * @param hi The high 64-bit value.
 * @param lo The low 64-bit value.
 * @return The 128-bit unsigned integer.
 */
static inline uint128_t make128(uint64_t hi, uint64_t lo) {
  return ((uint128_t)hi << 64) | (uint128_t)lo;
}

#else

/**
 * Gets the n-th bit of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @param n The bit index (0-127).
 * @return The value of the n-th bit (0 or 1).
 */
static inline uint64_t get_lo128(uint128_t v) {
  return v.lo;
}

/**
 * Gets the n-th bit of a 128-bit unsigned integer.
 *
 * @param v The 128-bit unsigned integer.
 * @param n The bit index (0-127).
 * @return The value of the n-th bit (0 or 1).
 */
static inline uint64_t get_hi128(uint128_t v) {
  return v.hi;
}

/**
 * Constructs a 128-bit unsigned integer from a high and low 64-bit values.
 *
 * @param hi The high 64-bit value.
 * @param lo The low 64-bit value.
 * @return The 128-bit unsigned integer.
 */
static inline uint128_t make128(uint64_t hi, uint64_t lo) {
  uint128_t r;
  r.hi = hi;
  r.lo = lo;
  return r;
}

#endif

#if LCG_UINT128_HAS_NATIVE

/**
 * Divides a 128-bit unsigned integer by 10 and returns the remainder.
 * using bit-by-bit long division.
 *
 * @param v Pointer to the 128-bit unsigned integer (updated to quotient).
 * @return The remainder (0-9).
 */
static inline unsigned int divmod10_128(uint128_t* v) {
  unsigned int rem = (unsigned int)(*v % 10);
  *v = *v / 10;
  return rem;
}

#else

/**
 * Gets the n-th bit of a 128-bit unsigned integer.
 *
 * This code was been generated with AI assistance.
 *
 * @param v The 128-bit unsigned integer.
 * @param n The bit index (0-127).
 * @return The value of the n-th bit (0 or 1).
 */
static inline int get_bit128(uint128_t v, int n) {
  if (n >= 64)
    return (int)((v >> (n - 64)) & 1);
  else
    return (int)((v >> n) & 1);
}

/**
 * Sets the n-th bit of a 128-bit unsigned integer to 1.
 *
 * This code was been generated with AI assistance.
 *
 * @param v Pointer to the 128-bit unsigned integer.
 * @param n The bit index (0-127).
 */
static inline void set_bit128(uint128_t* v, int n) {
  if (n >= 64)
    *v |= ((uint128_t)1 << (n - 64));
  else
    *v |= ((uint128_t)1 << n);
}

/**
 * Divides a 128-bit unsigned integer by 10 and returns the remainder.
 * using bit-by-bit long division.
 *
 * This code was been generated with AI assistance.
 *
 * @param v Pointer to the 128-bit unsigned integer (updated to quotient).
 * @return The remainder (0-9).
 */
static inline unsigned int divmod10_128(uint128_t* v) {
  uint128_t q = 0;
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

#endif

/**
 * Prints a 128-bit unsigned integer in decimal format.
 *
 * This code was been generated with AI assistance.
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
 * Based on the article "Optimal multipliers for pseudo-random number
 * generation by the linear congruential method" by Borosh and Niederreiter
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
static __global__ __launch_bounds__(256, 2) void search_kernel_128(uint128_t start_lambda, uint128_t* d_candidates, int* d_count) {
  unsigned long long idx = blockIdx.x * blockDim.x + threadIdx.x;
  uint128_t offset = (uint128_t)idx * 8;

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
    fprintf(f, "%llx %llx %llu %f\n", (unsigned long long)get_hi128(lambda), (unsigned long long)get_lo128(lambda), batches, accum_time);
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
      *lambda = make128(hi, lo);
      fclose(f);
      return true;
    }
    fclose(f);
  }
  return false;
}

void run_128_bit_search(void) {
  printf("--- 128-Bit Mode: GPU Fast-Filter & CPU Spectral Test ---\n\n");

  GpuConfig cfg = optimize_launch_config(search_kernel_128);
  print_gpu_info(cfg);

  uint128_t start_lambda;
  unsigned long long batches = 0;
  double accumulated_time = 0.0;
  const char* checkpoint_file = "search_state_128.txt";

  if (load_checkpoint_128(checkpoint_file, &start_lambda, &batches, &accumulated_time)) {
    printf("Found checkpoint! Resuming search...\n");
    printf("Starting from: 0x%016llx%016llx\n", (unsigned long long)get_hi128(start_lambda), (unsigned long long)get_lo128(start_lambda));
    printf("Time previously spent: ");
    format_and_print_time(accumulated_time);
    printf("\n\n");
  } else {
    printf("No checkpoint found. Starting fresh from Golden Ratio...\n\n");
    start_lambda = make128(GOLDEN_RATIO_128_HI, GOLDEN_RATIO_128_LO);
  }

  int* d_count[2];
  uint128_t* d_candidates[2];
  cudaStream_t streams[2];

  for (int i = 0; i < 2; i++) {
    cudaMalloc((void**)&d_count[i], sizeof(int));
    cudaMalloc((void**)&d_candidates[i], MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t));
    cudaStreamCreate(&streams[i]);
  }

  int* h_count[2];
  uint128_t* h_candidates[2];
  for (int i = 0; i < 2; i++) {
    cudaError_t err1 = cudaMallocHost((void**)&h_count[i], sizeof(int));
    cudaError_t err2 = cudaMallocHost((void**)&h_candidates[i], MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t));
    if (err1 != cudaSuccess || err2 != cudaSuccess) {
      fprintf(stderr, "Error: cudaMallocHost failed: %s\n", cudaGetErrorName(err1 != cudaSuccess ? err1 : err2));
      fprintf(stderr, "Falling back to malloc (pinned memory unavailable)\n");
      if (err1 != cudaSuccess) h_count[i] = (int*)malloc(sizeof(int));
      if (err2 != cudaSuccess) h_candidates[i] = (uint128_t*)malloc(MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t));
    }
    h_count[i][0] = 0;
  }

  uint128_t lambda_stride_per_batch = (uint128_t)compute_batch_stride(cfg);
  int cur = 0;

  time_t session_start_time = time(NULL);
  time_t last_checkpoint_time = session_start_time;
  unsigned long long last_batches = batches;
  bool found = false;

  cudaMemsetAsync(d_count[cur], 0, sizeof(int), streams[cur]);
  search_kernel_128<<<cfg.grid_size, cfg.block_size, 0, streams[cur]>>>(start_lambda, d_candidates[cur], d_count[cur]);
  cudaMemcpyAsync(h_count[cur], d_count[cur], sizeof(int), cudaMemcpyDeviceToHost, streams[cur]);
  cudaMemcpyAsync(h_candidates[cur], d_candidates[cur], MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t), cudaMemcpyDeviceToHost, streams[cur]);
  cudaStreamSynchronize(streams[cur]);

  while (!found) {
    int next = 1 - cur;
    uint128_t next_lambda = add128(start_lambda, lambda_stride_per_batch);

    cudaMemsetAsync(d_count[next], 0, sizeof(int), streams[next]);
    search_kernel_128<<<cfg.grid_size, cfg.block_size, 0, streams[next]>>>(next_lambda, d_candidates[next], d_count[next]);

    cudaMemcpyAsync(h_count[cur], d_count[cur], sizeof(int), cudaMemcpyDeviceToHost, streams[cur]);
    cudaMemcpyAsync(h_candidates[cur], d_candidates[cur], MAX_CANDIDATES_PER_BATCH * sizeof(uint128_t), cudaMemcpyDeviceToHost, streams[cur]);
    cudaStreamSynchronize(streams[cur]);

    if (*h_count[cur] > 0) {
      int process_count = (*h_count[cur] > MAX_CANDIDATES_PER_BATCH) ? MAX_CANDIDATES_PER_BATCH : *h_count[cur];
      for (int i = 0; i < process_count; i++) {
        if (passes_higher_dimensions_128(h_candidates[cur][i])) {
          double total_time = accumulated_time + difftime(time(NULL), session_start_time);

          printf("\n==========================================\n");
          printf("SUCCESS! Found 128-bit Multiplier passing ALL dimensions!\n");
          printf("Lambda (Hex): 0x%016llx%016llx\n", (unsigned long long)get_hi128(h_candidates[cur][i]), (unsigned long long)get_lo128(h_candidates[cur][i]));
          fflush(stdout);
          printf("Lambda (Dec): ");
          print128_dec(h_candidates[cur][i]);
          fflush(stdout);
          printf("\n\nTOTAL SEARCH TIME: ");
          format_and_print_time(total_time);
          printf("\n==========================================\n");
          fflush(stdout);

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
      save_checkpoint_128(checkpoint_file, start_lambda, batches, accumulated_time + current_session_time);

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
