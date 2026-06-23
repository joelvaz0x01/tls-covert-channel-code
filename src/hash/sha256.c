/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 */

#include <stddef.h>
#include <string.h>

#include "sha256.h"

const hash_algo_t sha256 = {
  .name = "SHA-256",
  .digest_size = 32,
  .context_size = sizeof(sha256_ctx_t),
  .init = sha256_init,
  .update = sha256_update,
  .final = sha256_final
};

/**
 * Right rotation of a 32-bit word.
 *
 * @param a The word to rotate.
 * @param b The number of bits to rotate.
 * @return The rotated word.
 */
FN_ uint32_t rotright(uint32_t a, uint32_t b) {
  return (a >> b) | (a << (32 - b));
}

/**
 * SHA-256 Ch function: bits from x and y are chosen based on x.
 *
 * @param x Control word.
 * @param y Selection word 1.
 * @param z Selection word 2.
 * @return The result of (x AND y) XOR (NOT x AND z).
 */
FN_ uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (~x & z);
}

/**
 * SHA-256 Maj function: returns the bit that appears in at least two of the three inputs.
 *
 * @param x Input word 1.
 * @param y Input word 2.
 * @param z Input word 3.
 * @return The result of (x AND y) XOR (x AND z) XOR (y AND z).
 */
FN_ uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

/**
 * SHA-256 EP0 function (Upper Sigma 0).
 *
 * @param x Input word.
 * @return Result of ROTR(x, 2) XOR ROTR(x, 13) XOR ROTR(x, 22).
 */
FN_ uint32_t ep0(uint32_t x) {
  return rotright(x, 2) ^ rotright(x, 13) ^ rotright(x, 22);
}

/**
 * SHA-256 EP1 function (Upper Sigma 1).
 *
 * @param x Input word.
 * @return Result of ROTR(x, 6) XOR ROTR(x, 11) XOR ROTR(x, 25).
 */
FN_ uint32_t ep1(uint32_t x) {
  return rotright(x, 6) ^ rotright(x, 11) ^ rotright(x, 25);
}

/**
 * SHA-256 SIG0 function (Lower Sigma 0).
 *
 * @param x Input word.
 * @return Result of ROTR(x, 7) XOR ROTR(x, 18) XOR (x >> 3).
 */
FN_ uint32_t sig0(uint32_t x) {
  return rotright(x, 7) ^ rotright(x, 18) ^ (x >> 3);
}

/**
 * SHA-256 SIG1 function (Lower Sigma 1).
 *
 * @param x Input word.
 * @return Result of ROTR(x, 17) XOR ROTR(x, 19) XOR (x >> 10).
 */
FN_ uint32_t sig1(uint32_t x) {
  return rotright(x, 17) ^ rotright(x, 19) ^ (x >> 10);
}

static const uint32_t k[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * Performs a single SHA-256 transformation on the given data block.
 *
 * @param ctx The SHA-256 context to update.
 * @param data The data block to hash.
 */
static void sha256_transform(sha256_ctx_t* ctx, const uint8_t data[]) {
  uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

  for (i = 0, j = 0; i < 16; ++i, j += 4)
    m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
  for (; i < 64; ++i) m[i] = sig1(m[i - 2]) + m[i - 7] + sig0(m[i - 15]) + m[i - 16];

  a = ctx->state[0];
  b = ctx->state[1];
  c = ctx->state[2];
  d = ctx->state[3];
  e = ctx->state[4];
  f = ctx->state[5];
  g = ctx->state[6];
  h = ctx->state[7];

  for (i = 0; i < 64; ++i) {
    t1 = h + ep1(e) + ch(e, f, g) + k[i] + m[i];
    t2 = ep0(a) + maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}

int sha256_init(void* ctx) {
  if (NULL == ctx) return 0;
  sha256_ctx_t* s_ctx = (sha256_ctx_t*)ctx;
  memset(s_ctx->data, 0, 64);
  s_ctx->bitlen_buffer = 0;
  s_ctx->bitlen_total = 0;
  s_ctx->state[0] = 0x6a09e667;
  s_ctx->state[1] = 0xbb67ae85;
  s_ctx->state[2] = 0x3c6ef372;
  s_ctx->state[3] = 0xa54ff53a;
  s_ctx->state[4] = 0x510e527f;
  s_ctx->state[5] = 0x9b05688c;
  s_ctx->state[6] = 0x1f83d9ab;
  s_ctx->state[7] = 0x5be0cd19;
  return 1;
}

int sha256_update(void* ctx, const void* data, size_t bit_len) {
  if (NULL == ctx || (NULL == data && bit_len > 0)) return 0;
  sha256_ctx_t* s_ctx = (sha256_ctx_t*)ctx;
  const uint8_t* u_data = (const uint8_t*)data;
  size_t i = 0;

  while (i < bit_len) {
    uint32_t buffer_bit_offset = s_ctx->bitlen_buffer % 8;
    uint32_t bits_to_copy;

    if (buffer_bit_offset == 0 && (bit_len - i) >= 8) {
      // Aligned byte copy
      s_ctx->data[s_ctx->bitlen_buffer / 8] = u_data[i / 8];
      bits_to_copy = 8;
    } else {
      // Bit-by-bit copy for unaligned or partial bits
      uint32_t bit = (u_data[i / 8] >> (7 - (i % 8))) & 1;
      uint8_t mask = (0x80 >> buffer_bit_offset);
      if (bit) {
        s_ctx->data[s_ctx->bitlen_buffer / 8] |= mask;
      } else {
        s_ctx->data[s_ctx->bitlen_buffer / 8] &= ~mask;
      }
      bits_to_copy = 1;
    }

    s_ctx->bitlen_buffer += bits_to_copy;
    s_ctx->bitlen_total += bits_to_copy;
    i += bits_to_copy;

    if (s_ctx->bitlen_buffer == 512) {
      sha256_transform(s_ctx, s_ctx->data);
      s_ctx->bitlen_buffer = 0;
      memset(s_ctx->data, 0, 64);
    }
  }
  return 1;
}

int sha256_final(void* ctx, uint8_t* digest) {
  if (NULL == ctx || NULL == digest) return 0;
  sha256_ctx_t* s_ctx = (sha256_ctx_t*)ctx;
  uint32_t i;
  uint64_t total_bits = s_ctx->bitlen_total;

  uint32_t buffer_bit_offset = s_ctx->bitlen_buffer % 8;
  uint8_t mask = (0xFF << (8 - buffer_bit_offset)) & 0xFF;
  if (buffer_bit_offset == 0) mask = 0;
  s_ctx->data[s_ctx->bitlen_buffer / 8] &= mask;
  s_ctx->data[s_ctx->bitlen_buffer / 8] |= (0x80 >> buffer_bit_offset);
  s_ctx->bitlen_buffer++;

  /* pad with '0' bits until bit 448 (mod 512) */
  if (s_ctx->bitlen_buffer > 448) {
    while (s_ctx->bitlen_buffer < 512) {
      if (s_ctx->bitlen_buffer % 8 == 0) {
        s_ctx->data[s_ctx->bitlen_buffer / 8] = 0;
        s_ctx->bitlen_buffer += 8;
      } else {
        s_ctx->data[s_ctx->bitlen_buffer / 8] &= ~(0x80 >> (s_ctx->bitlen_buffer % 8));
        s_ctx->bitlen_buffer++;
      }
    }
    sha256_transform(s_ctx, s_ctx->data);
    s_ctx->bitlen_buffer = 0;
    memset(s_ctx->data, 0, 64);
  }

  while (s_ctx->bitlen_buffer < 448) {
    if (s_ctx->bitlen_buffer % 8 == 0) {
      s_ctx->data[s_ctx->bitlen_buffer / 8] = 0;
      s_ctx->bitlen_buffer += 8;
    } else {
      s_ctx->data[s_ctx->bitlen_buffer / 8] &= ~(0x80 >> (s_ctx->bitlen_buffer % 8));
      s_ctx->bitlen_buffer++;
    }
  }

  s_ctx->data[56] = (uint8_t)(total_bits >> 56);
  s_ctx->data[57] = (uint8_t)(total_bits >> 48);
  s_ctx->data[58] = (uint8_t)(total_bits >> 40);
  s_ctx->data[59] = (uint8_t)(total_bits >> 32);
  s_ctx->data[60] = (uint8_t)(total_bits >> 24);
  s_ctx->data[61] = (uint8_t)(total_bits >> 16);
  s_ctx->data[62] = (uint8_t)(total_bits >> 8);
  s_ctx->data[63] = (uint8_t)total_bits;

  sha256_transform(s_ctx, s_ctx->data);

  for (i = 0; i < 4; ++i) {
    digest[i] = (s_ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 4] = (s_ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 8] = (s_ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 12] = (s_ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 16] = (s_ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 20] = (s_ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 24] = (s_ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
    digest[i + 28] = (s_ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
  }
  return 1;
}
