/**
 * @file fountain_code.c
 * @brief Implementation of the fountain code file-transfer library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fountain_code/encoder.h>
#include <fountain_code/decoder.h>

/**
 * Zeroes out the given vector.
 *
 * @param v Pointer to the vector to zero out.
 * @param n_words Number of words in the vector.
 */
static void vec_zero(vec_t *v, int n_words)
{
  for (int i = 0; i < n_words; i++) v->w[i] = 0ul;
}

/**
 * Sets the given bit in the vector.
 *
 * @param v Pointer to the vector to set.
 * @param bit Index of the bit to set.
 */
static void vec_set(vec_t *v, int bit)
{
  v->w[bit / 64] |= 1ul << (bit % 64);
}

/**
 * Tests the given bit in the vector.
 *
 * @param v Pointer to the vector to test.
 * @param bit Index of the bit to test.
 * @return 1 if the bit is set, 0 otherwise.
 */
static int vec_test(const vec_t *v, int bit)
{
  return (int)((v->w[bit / 64] >> (bit % 64)) & 1ul);
}

/**
 * Performs XOR operation on two vectors.
 *
 * @param dst Pointer to the destination vector.
 * @param src Pointer to the source vector.
 * @param n_words Number of words in the vectors.
 */
static void vec_xor(vec_t *dst, const vec_t *src, int n_words)
{
  for (int i = 0; i < n_words; i++) dst->w[i] ^= src->w[i];
}

/**
 * Returns the index of the lowest set bit in the vector, or -1 if the vector is all-zero.
 *
 * @param v Pointer to the vector to test.
 * @param n_words Number of words in the vector.
 * @return Index of the lowest set bit, or -1 if the vector is all-zero.
 */
static int vec_lsb(const vec_t *v, int n_words)
{
  for (int i = 0; i < n_words; i++)
    if (0ul != v->w[i])
      return 64 * i + (int)__builtin_ctzl(v->w[i]);
  return -1;
}

/**
 * Performs XOR operation on two byte arrays.
 *
 * @param dst Pointer to the destination byte array.
 * @param src Pointer to the source byte array.
 */
static void data_xor(unsigned char *dst, const unsigned char *src)
{
  for (int i = 0; i < BLOCK_SIZE; i++) dst[i] ^= src[i];
}

void print_sel(const vec_t *v, int n)
{
  for (int i = 0; i < n; i++) putchar(vec_test(v, i) ? '1' : '0');
}

packet_t encode_packet(int id, const unsigned char *blocks, int n, int m, int n_words)
{
  packet_t pkt;
  pkt.id = id;
  vec_zero(&pkt.selector, n_words);
  memset(pkt.data, 0, BLOCK_SIZE);

  for (int i = 0; i < m; i++) {
    int j = (int)(((unsigned long)(unsigned int)rand() + 314159311ul * (unsigned long)(unsigned int)rand()) % (unsigned long)n);
    vec_set(&pkt.selector, j);
  }

  for (int j = 0; j < n; j++)
    if (vec_test(&pkt.selector, j))
      data_xor(pkt.data, blocks + (size_t)j * BLOCK_SIZE);

  return pkt;
}

void decoder_init(decoder_t *dec, int n)
{
  memset(dec, 0, sizeof(*dec));
  dec->n         = n;             /* number of source blocks           */
  dec->n_words   = (n + 63) / 64; /* number of word-sized selector vec */
  dec->remaining = n;             /* number of blocks still needed     */
}

int decoder_feed(decoder_t *dec, const packet_t *pkt)
{
  vec_t         sel;
  unsigned char dat[BLOCK_SIZE];

  sel = pkt->selector;
  memcpy(dat, pkt->data, BLOCK_SIZE);

  for (;;) {
    int i = vec_lsb(&sel, dec->n_words);
    if (i < 0) return 0;            /* all bits cancelled — redundant     */

    if (!dec->pivot_present[i]) {   /* new leading bit: store as pivot    */
      dec->pivot_present[i] = 1;
      dec->pivot_sel[i]     = sel;
      memcpy(dec->pivot_data[i], dat, BLOCK_SIZE);
      dec->remaining--;
      return 1;
    }
    /* eliminate the leading bit using the existing pivot at position i.  */
    vec_xor(&sel, &dec->pivot_sel[i], dec->n_words);
    data_xor(dat, dec->pivot_data[i]);
  }
}

void decoder_solve(decoder_t *dec, unsigned char *out_blocks)
{
  int n = dec->n, nw = dec->n_words;

  for (int i = n - 1; i >= 0; i--) {
    if (!dec->pivot_present[i]) continue;
    for (int j = i + 1; j < n; j++) {
      if (!dec->pivot_present[j]) continue;
      if (!vec_test(&dec->pivot_sel[i], j)) continue;
      vec_xor(&dec->pivot_sel[i], &dec->pivot_sel[j], nw);
      data_xor(dec->pivot_data[i], dec->pivot_data[j]);
    }
  }

  /* each pivot[i] now represents exactly source block i. */
  for (int i = 0; i < n; i++) {
    if (dec->pivot_present[i])
      memcpy(out_blocks + (size_t)i * BLOCK_SIZE, dec->pivot_data[i], BLOCK_SIZE);
    else
      memset(out_blocks + (size_t)i * BLOCK_SIZE, 0, BLOCK_SIZE);
  }
}
