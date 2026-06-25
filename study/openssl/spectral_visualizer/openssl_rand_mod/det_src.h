/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance based on
 * OpenSSL 3.6.3 source to produce a deterministic RAND_bytes_ex
 */

#ifndef OPENSSL_RAND_MOD_DET_SRC_H
#define OPENSSL_RAND_MOD_DET_SRC_H

#include <openssl/core.h>
#include <openssl/core_dispatch.h>

/*
 * @struct det_ctx
 * Opaque context for one DET-SRC instance
 *
 * @var provctx  Provider context handle
 * @var state    Current RAND state (UNINITIALISED / READY)
 */
typedef struct {
  void* provctx;
  int state;
} det_ctx;

/*
 * Built-in provider initialisation for the "det" provider
 *
 * Called by OpenSSL when the "det" provider is loaded. Sets the
 * provider dispatch table so OpenSSL can discover the DET-SRC
 * algorithm via det_query().
 *
 * @param handle OSSL_CORE_HANDLE for this provider (unused)
 * @param in Core dispatch table (unused)
 * @param out Output provider dispatch table
 * @param provctx Output provider context (unused)
 * @return 1 on success
 */
int det_provider_init(const OSSL_CORE_HANDLE* handle, const OSSL_DISPATCH* in, const OSSL_DISPATCH** out, void** provctx);

#endif /* OPENSSL_RAND_MOD_DET_SRC_H */
