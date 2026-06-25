/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance based on
 * OpenSSL 3.6.3 source to produce a deterministic RAND_bytes_ex
 */

#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>
#include <openssl/provider.h>

#include <stddef.h>
#include <time.h>

#include "det_src.h"
#include "seed_setup.h"

static EVP_RAND_CTX* g_det_src = NULL;
static EVP_RAND_CTX* g_det_drbg = NULL;

int DET_RAND_bytes_ex(OSSL_LIB_CTX* ctx, unsigned char* buf, size_t size, unsigned int strength) {
  if (g_det_drbg != NULL)
    return EVP_RAND_generate(g_det_drbg, buf, size, strength, 0, NULL, 0) ? 1 : 0;

  /* on first call, deterministic DRBG chain will be created */

  /* fetch and create DET-SRC seed source as root */
  EVP_RAND* det_rand = EVP_RAND_fetch(ctx, "DET-SRC", "provider=det");
  if (det_rand == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }
  g_det_src = EVP_RAND_CTX_new(det_rand, NULL);
  EVP_RAND_free(det_rand);
  if (g_det_src == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }
  if (!EVP_RAND_instantiate(g_det_src, 0, 0, NULL, 0, NULL)) {
    ERR_print_errors_fp(stderr);
    EVP_RAND_CTX_free(g_det_src);
    g_det_src = NULL;
    return 0;
  }

  /* fetch and create CTR-DRBG with DET-SRC as parent */
  EVP_RAND* ctr_rand = EVP_RAND_fetch(ctx, "CTR-DRBG", "provider=default");
  if (ctr_rand == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }
  g_det_drbg = EVP_RAND_CTX_new(ctr_rand, g_det_src);
  EVP_RAND_free(ctr_rand);
  if (g_det_drbg == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }

  /* configure CTR-DRBG parameters */
  unsigned int reseed_requests = 0;
  time_t reseed_time = 0;
  int use_df = 1;
  OSSL_PARAM params[5];
  int i = 0;
  params[i++] = OSSL_PARAM_construct_int(OSSL_DRBG_PARAM_USE_DF, &use_df);
  params[i++] = OSSL_PARAM_construct_utf8_string(OSSL_DRBG_PARAM_CIPHER, "AES-256-CTR", 0);
  params[i++] = OSSL_PARAM_construct_uint(OSSL_DRBG_PARAM_RESEED_REQUESTS, &reseed_requests);
  params[i++] = OSSL_PARAM_construct_time_t(
    OSSL_DRBG_PARAM_RESEED_TIME_INTERVAL, &reseed_time
  );
  params[i++] = OSSL_PARAM_construct_end();

  /* instantiate – to ignore ASLR-dependent additional input (ADIN) */
  if (!EVP_RAND_instantiate(g_det_drbg, 256, 0, NULL, 0, params)) {
    ERR_print_errors_fp(stderr);
    EVP_RAND_CTX_free(g_det_drbg);
    g_det_drbg = NULL;
    return 0;
  }

  /* first generate */
  return EVP_RAND_generate(g_det_drbg, buf, size, strength, 0, NULL, 0) ? 1 : 0;
}

int DET_RAND_bytes_register(OSSL_LIB_CTX* ctx) {
  if (!OSSL_PROVIDER_add_builtin(ctx, "det", det_provider_init)) {
    return 0;
  }

  if (OSSL_PROVIDER_load(ctx, "det") == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }

  /* load the default provider so CTR-DRBG / AES-256-CTR are available */
  if (OSSL_PROVIDER_load(ctx, "default") == NULL) {
    ERR_print_errors_fp(stderr);
    return 0;
  }

  return 1;
}
