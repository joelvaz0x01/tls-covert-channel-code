/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance based on
 * OpenSSL 3.6.3 source to produce a deterministic RAND_bytes_ex
 */

#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/params.h>

#include <stdint.h>
#include <string.h>

#include "det_src.h"
#include "settings.h"

static OSSL_FUNC_rand_newctx_fn det_newctx;
static OSSL_FUNC_rand_freectx_fn det_freectx;
static OSSL_FUNC_rand_instantiate_fn det_instantiate;
static OSSL_FUNC_rand_uninstantiate_fn det_uninstantiate;
static OSSL_FUNC_rand_generate_fn det_generate;
static OSSL_FUNC_rand_reseed_fn det_reseed;
static OSSL_FUNC_rand_get_seed_fn det_get_seed;
static OSSL_FUNC_rand_clear_seed_fn det_clear_seed;
static OSSL_FUNC_rand_verify_zeroization_fn det_verify_zeroization;
static OSSL_FUNC_rand_enable_locking_fn det_enable_locking;
static OSSL_FUNC_rand_lock_fn det_lock;
static OSSL_FUNC_rand_unlock_fn det_unlock;
static OSSL_FUNC_rand_get_ctx_params_fn det_get_ctx_params;
static OSSL_FUNC_rand_gettable_ctx_params_fn det_gettable_ctx_params;
static OSSL_FUNC_rand_set_ctx_params_fn det_set_ctx_params;
static OSSL_FUNC_rand_settable_ctx_params_fn det_settable_ctx_params;

static const OSSL_DISPATCH det_dispatch[] = {
  {OSSL_FUNC_RAND_NEWCTX, (void (*)(void))det_newctx},
  {OSSL_FUNC_RAND_FREECTX, (void (*)(void))det_freectx},
  {OSSL_FUNC_RAND_INSTANTIATE, (void (*)(void))det_instantiate},
  {OSSL_FUNC_RAND_UNINSTANTIATE, (void (*)(void))det_uninstantiate},
  {OSSL_FUNC_RAND_GENERATE, (void (*)(void))det_generate},
  {OSSL_FUNC_RAND_RESEED, (void (*)(void))det_reseed},
  {OSSL_FUNC_RAND_GET_SEED, (void (*)(void))det_get_seed},
  {OSSL_FUNC_RAND_CLEAR_SEED, (void (*)(void))det_clear_seed},
  {OSSL_FUNC_RAND_VERIFY_ZEROIZATION, (void (*)(void))det_verify_zeroization},
  {OSSL_FUNC_RAND_ENABLE_LOCKING, (void (*)(void))det_enable_locking},
  {OSSL_FUNC_RAND_LOCK, (void (*)(void))det_lock},
  {OSSL_FUNC_RAND_UNLOCK, (void (*)(void))det_unlock},
  {OSSL_FUNC_RAND_GETTABLE_CTX_PARAMS, (void (*)(void))det_gettable_ctx_params},
  {OSSL_FUNC_RAND_GET_CTX_PARAMS, (void (*)(void))det_get_ctx_params},
  {OSSL_FUNC_RAND_SETTABLE_CTX_PARAMS, (void (*)(void))det_settable_ctx_params},
  {OSSL_FUNC_RAND_SET_CTX_PARAMS, (void (*)(void))det_set_ctx_params},
  OSSL_DISPATCH_END
};

static void det_provider_teardown(void* provctx) {
  (void)provctx;
}

static const OSSL_ALGORITHM* det_query(void* provctx, int operation_id, int* no_cache) {
  (void)provctx;
  *no_cache = 0;
  if (operation_id == OSSL_OP_RAND) {
    static const OSSL_ALGORITHM det_algs[] = {
      {"DET-SRC", "provider=det", det_dispatch, NULL},
      {NULL, NULL, NULL, NULL}
    };
    return det_algs;
  }
  return NULL;
}

static const OSSL_DISPATCH det_provider_dispatch[] = {
  {OSSL_FUNC_PROVIDER_TEARDOWN, (void (*)(void))det_provider_teardown},
  {OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))det_query},
  OSSL_DISPATCH_END
};

static void* det_newctx(void* provctx, void* parent, const OSSL_DISPATCH* parent_calls) {
  (void)parent;
  (void)parent_calls;
  det_ctx* t = OPENSSL_zalloc(sizeof(*t));
  if (t == NULL)
    return NULL;
  t->provctx = provctx;
  t->state = EVP_RAND_STATE_UNINITIALISED;
  return t;
}

static void det_freectx(void* vctx) {
  OPENSSL_free(vctx);
}

static int det_instantiate(void* vdrbg, unsigned int strength, int prediction_resistance, const unsigned char* pstr, size_t pstr_len, const OSSL_PARAM params[]) {
  (void)strength;
  (void)prediction_resistance;
  (void)pstr;
  (void)pstr_len;
  (void)params;
  det_ctx* t = vdrbg;
  t->state = EVP_RAND_STATE_READY;
  return 1;
}

static int det_uninstantiate(void* vdrbg) {
  det_ctx* t = vdrbg;
  t->state = EVP_RAND_STATE_UNINITIALISED;
  return 1;
}

static int det_generate(void* vctx, unsigned char* out, size_t outlen, unsigned int strength, int prediction_resistance, const unsigned char* adin, size_t adin_len) {
  (void)vctx;
  (void)strength;
  (void)prediction_resistance;
  (void)adin;
  (void)adin_len;
  uint64_t pattern = (uint64_t)RAND_BYTES_SEED;
  for (size_t i = 0; i < outlen; i += sizeof(pattern)) {
    size_t chunk = outlen - i;
    if (chunk > sizeof(pattern))
      chunk = sizeof(pattern);
    memcpy(out + i, &pattern, chunk);
  }
  return 1;
}

static int det_reseed(void* vctx, int prediction_resistance, const unsigned char* ent, size_t ent_len, const unsigned char* adin, size_t adin_len) {
  (void)vctx;
  (void)prediction_resistance;
  (void)ent;
  (void)ent_len;
  (void)adin;
  (void)adin_len;
  return 1;
}

static size_t det_get_seed(void* vctx, unsigned char** buffer, int entropy, size_t min_len, size_t max_len, int prediction_resistance, const unsigned char* adin, size_t adin_len) {
  (void)vctx;
  (void)entropy;
  (void)prediction_resistance;
  (void)adin;
  (void)adin_len;
  size_t out_len = min_len;
  if (out_len > max_len)
    out_len = max_len;
  unsigned char* buf = OPENSSL_malloc(out_len);
  if (buf == NULL)
    return 0;

  uint64_t pattern = (uint64_t)RAND_BYTES_SEED;
  for (size_t i = 0; i < out_len; i += sizeof(pattern)) {
    size_t chunk = out_len - i;
    if (chunk > sizeof(pattern))
      chunk = sizeof(pattern);
    memcpy(buf + i, &pattern, chunk);
  }

  *buffer = buf;
  return out_len;
}

static void det_clear_seed(void* vctx, unsigned char* buffer, size_t b_len) {
  (void)vctx;
  OPENSSL_clear_free(buffer, b_len);
}

static int det_verify_zeroization(void* vctx) {
  (void)vctx;
  return 1;
}

static int det_enable_locking(void* vctx) {
  (void)vctx;
  return 1;
}

static int det_lock(void* vctx) {
  (void)vctx;
  return 1;
}

static void det_unlock(void* vctx) {
  (void)vctx;
}

static const OSSL_PARAM det_gettable_ctx_params_list[] = {
  OSSL_PARAM_int(OSSL_RAND_PARAM_STATE, NULL),
  OSSL_PARAM_uint(OSSL_RAND_PARAM_STRENGTH, NULL),
  OSSL_PARAM_END
};

static int det_get_ctx_params(void* vctx, OSSL_PARAM params[]) {
  det_ctx* t = vctx;
  OSSL_PARAM* p;

  p = OSSL_PARAM_locate(params, OSSL_RAND_PARAM_STATE);
  if (p != NULL && !OSSL_PARAM_set_int(p, t->state))
    return 0;

  p = OSSL_PARAM_locate(params, OSSL_RAND_PARAM_STRENGTH);
  if (p != NULL && !OSSL_PARAM_set_uint(p, 256))
    return 0;

  return 1;
}

static const OSSL_PARAM* det_gettable_ctx_params(void* vctx, void* provctx) {
  (void)vctx;
  (void)provctx;
  return det_gettable_ctx_params_list;
}

static const OSSL_PARAM det_settable_ctx_params_list[] = {
  OSSL_PARAM_uint(OSSL_RAND_PARAM_STRENGTH, NULL),
  OSSL_PARAM_END
};

static int det_set_ctx_params(void* vctx, const OSSL_PARAM params[]) {
  (void)vctx;
  (void)params;
  return 1;
}

static const OSSL_PARAM* det_settable_ctx_params(void* vctx, void* provctx) {
  (void)vctx;
  (void)provctx;
  return det_settable_ctx_params_list;
}

int det_provider_init(const OSSL_CORE_HANDLE* handle, const OSSL_DISPATCH* in, const OSSL_DISPATCH** out, void** provctx) {
  (void)handle;
  (void)in;
  (void)provctx;
  *out = det_provider_dispatch;
  return 1;
}
