/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance
 */

#ifndef LCG_MULT_FINDER_GPU_CONFIG_H
#define LCG_MULT_FINDER_GPU_CONFIG_H

#include <cstdio>
#include <cuda_runtime.h>

constexpr int WAVE_MULTIPLIER = 32;

struct GpuConfig {
  int block_size;
  int grid_size;
  int sm_count;
  int compute_major;
  int compute_minor;
  int max_threads_per_sm;
  int warp_size;
  int active_blocks_per_sm;
  int waves;
  unsigned long long threads_per_batch;
  char device_name[256];
};

inline void print_gpu_info(const GpuConfig& cfg) {
  printf("=== GPU Configuration ===\n");
  printf("Device:              %s\n", cfg.device_name);
  printf("Compute Capability:  %d.%d\n", cfg.compute_major, cfg.compute_minor);
  printf("SMs:                 %d\n", cfg.sm_count);
  printf("Max Threads/SM:      %d\n", cfg.max_threads_per_sm);
  printf("Warp Size:           %d\n", cfg.warp_size);
  printf("Block Size:          %d\n", cfg.block_size);
  printf("Active Blocks/SM:    %d\n", cfg.active_blocks_per_sm);
  printf("Grid Size:           %d\n", cfg.grid_size);
  printf("Waves:               %d\n", cfg.waves);
  printf("Threads per Batch:   %llu\n", cfg.threads_per_batch);
  printf("=========================\n\n");
}

/**
 * Optimizes the launch configuration for the given kernel function.
 *
 * @param kernel The kernel function to optimize.
 * @param dynamic_smem The dynamic shared memory size (optional).
 * @return The optimized launch configuration.
 */
template <typename KernelFunc>
inline GpuConfig optimize_launch_config(KernelFunc kernel, size_t dynamic_smem = 0) {
  GpuConfig cfg = {};

  int device;
  cudaError_t err = cudaGetDevice(&device);
  if (err != cudaSuccess) {
    printf("Error: cudaGetDevice failed: %s\n", cudaGetErrorName(err));
    return cfg;
  }

  cudaDeviceProp prop;
  err = cudaGetDeviceProperties(&prop, device);
  if (err != cudaSuccess) {
    printf("Error: cudaGetDeviceProperties failed: %s\n", cudaGetErrorName(err));
    return cfg;
  }

  cfg.sm_count = prop.multiProcessorCount;
  cfg.compute_major = prop.major;
  cfg.compute_minor = prop.minor;
  cfg.max_threads_per_sm = prop.maxThreadsPerMultiProcessor;
  cfg.warp_size = prop.warpSize;
  snprintf(cfg.device_name, sizeof(cfg.device_name), "%s", prop.name);

  int block_size = 0;
  int min_grid_size = 0;
  cudaOccupancyMaxPotentialBlockSize(&min_grid_size, &block_size, (const void*)kernel, dynamic_smem);

  int active_blocks_per_sm = 0;
  cudaOccupancyMaxActiveBlocksPerMultiprocessor(&active_blocks_per_sm, (const void*)kernel, block_size, dynamic_smem);

  int concurrent_blocks = active_blocks_per_sm * cfg.sm_count;
  int grid_size = concurrent_blocks * WAVE_MULTIPLIER;

  cfg.block_size = block_size;
  cfg.grid_size = grid_size;
  cfg.active_blocks_per_sm = active_blocks_per_sm;
  cfg.waves = WAVE_MULTIPLIER;
  cfg.threads_per_batch = (unsigned long long)grid_size * block_size;

  return cfg;
}

/**
 * Computes the batch stride for the given GPU configuration.
 *
 * @param cfg The GPU configuration.
 * @return The batch stride.
 */
inline unsigned long long compute_batch_stride(const GpuConfig& cfg) {
  return cfg.threads_per_batch * 8;
}

#endif /* LCG_MULT_FINDER_GPU_CONFIG_H */
