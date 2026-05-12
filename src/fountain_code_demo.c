/**
 * @file fountain-code-demo.c
 * @brief Toy example that demonstrates the fountain code encoder and decoder.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fountain_code/encoder.h>
#include <fountain_code/decoder.h>
#include <utils/print_byte_arrays.h>

int main(void)
{
  const char *input_file  = "input.txt";
  const char *output_file = "reconstructed.txt";

  /* ensure 'input.txt' exists and has content. */
  {
    FILE *chk = fopen(input_file, "rb");
    int needs_content;
    if (NULL == chk) {
      needs_content = 1;  /* file does not exist */
    } else {
      fseek(chk, 0, SEEK_END);
      needs_content = (0 == ftell(chk));  /* file exists but is empty */
      fclose(chk);
    }

    /* put default text into empty file */
    if (needs_content) {
      FILE *wfp = fopen(input_file, "w");
      if (!wfp) { perror("fopen input.txt"); return 1; }
      fputs(
        "This is a live fountain code file-transfer demonstration made for "
        "'Low-Bandwidth Covert Channels Using The Initial TLS Handshake Messages' dissertation.\n"
        "2026 Joel Vaz @ Universidade de Aveiro\n", wfp
      );
      fclose(wfp);
    }
  }

  /* read input file into zero-padded source blocks */
  FILE *fp = fopen(input_file, "rb");
  if (!fp) { perror("fopen"); return 1; }
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);

  int n = (int)((file_size + BLOCK_SIZE - 1) / BLOCK_SIZE);
  if (n < 2 || n > MAX_BLOCKS) {
    fprintf(stderr, "File must produce between 2 and %d blocks.\n", MAX_BLOCKS);
    fclose(fp);
    return 1;
  }

  unsigned char *src = calloc((size_t)n, BLOCK_SIZE); /* create zero-padded source blocks */
  if (!src) { perror("calloc"); fclose(fp); return 1; }

  if (fread(src, 1, (size_t)file_size, fp) != (size_t)file_size) {
    perror("fread"); free(src); fclose(fp); return 1;
  }
  fclose(fp);

  /* compute packet degree m */
  int m = (int)round(2.0 * log((double)n) + EULER);
  if (m < 1) m = 1;
  if (m >= n) m = n - 1;

  /* print banner */
  printf("======================================================================\n");
  printf(" Fountain Code File-Transfer Demo\n");
  printf("======================================================================\n");
  printf(" Input file    : %s (%ld bytes)\n", input_file, file_size);
  printf(" Source blocks : n = %d blocks (each with %d bytes)\n", n, BLOCK_SIZE);
  printf(" Degree        : m = round(2 * ln(%d) + Euler-Mascheroni)\n", n);
  printf("======================================================================\n\n");

  /* print source blocks */
  printf("[ Source blocks ]\n");
  for (int i = 0; i < n; i++) {
    printf("  [%2d]  hex: ", i);
    print_hex(src + (size_t)i * BLOCK_SIZE, BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii(src + (size_t)i * BLOCK_SIZE, BLOCK_SIZE);
    printf("\"\n");
  }
  putchar('\n');

  /* encode packets and decode on-the-fly */
  srand((unsigned int)time(NULL));

  decoder_t *dec = calloc(1, sizeof(decoder_t));
  if (!dec) { perror("calloc decoder"); free(src); return 1; }
  decoder_init(dec, n);

  int n_words      = (n + 63) / 64;
  int total_sent   = 0;
  int total_useful = 0;

  /* column header */
  printf("[ Fountain packets ]\n");
  printf(
    "  %-4s  %-*s  %-*s  %s\n",
    "Pkt#", n,
    "Selector", BLOCK_SIZE * 2,
    "Encoded data (hex)",
    "Status"
  );

  /* separator line */
  printf("  ");
  for (int k = 0; k < 6 + n + 2 + BLOCK_SIZE * 2 + 2 + 30; k++) putchar('-');
  putchar('\n');

  while (dec->remaining > 0) {
    packet_t pkt = encode_packet(total_sent, src, n, m, n_words);
    total_sent++;

    int useful = decoder_feed(dec, &pkt);
    if (useful) total_useful++;

    printf("  #%-3d  ", pkt.id);
    print_sel(&pkt.selector, n);
    printf("  ");
    print_hex(pkt.data, BLOCK_SIZE);
    printf(
      "  %s  (remaining=%d)\n",
      useful ? "[ new pivot ]" : "[ redundant ]",
      dec->remaining
    );
  }

  printf(
    "\n  Packets sent: %d  |  useful (new pivot): %d  |  redundant: %d\n\n",
    total_sent, total_useful, total_sent - total_useful
  );

  /* back-substitution to recover individual source blocks */
  unsigned char *out = calloc((size_t)n, BLOCK_SIZE);
  if (!out) { perror("calloc out"); free(dec); free(src); return 1; }

  decoder_solve(dec, out);

  /* print reconstructed blocks */
  printf("[ Reconstructed blocks ]\n");
  int all_ok = 1;
  for (int i = 0; i < n; i++) {
    int ok = (memcmp(out + (size_t)i * BLOCK_SIZE, src + (size_t)i * BLOCK_SIZE, BLOCK_SIZE) == 0);
    if (!ok) all_ok = 0;
    printf("  [%2d]  hex: ", i);
    print_hex(out + (size_t)i * BLOCK_SIZE, BLOCK_SIZE);
    printf("  txt: \"");
    print_ascii(out + (size_t)i * BLOCK_SIZE, BLOCK_SIZE);
    printf("\"  [%s]\n", ok ? " OK " : "FAIL");
  }

  /* write output file */
  fp = fopen(output_file, "wb");
  if (fp) {
    fwrite(out, 1, (size_t)file_size, fp);
    fclose(fp);
  }

  printf(
    "\n  Reconstruction : %s\n",
    all_ok
      ? "SUCCESS — original file recovered perfectly!"
      : "FAILURE — one or more blocks do not match!"
  );
  printf("  Output written : %s\n\n", output_file);

  free(out);
  free(dec);
  free(src);

  return all_ok ? 0 : 1;
}
