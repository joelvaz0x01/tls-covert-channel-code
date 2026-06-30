/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * detect the version of loaded OpenSSL library.
 */

#ifndef OPENSSL_OFFSET_OPENSSL_DETECT_H
#define OPENSSL_OFFSET_OPENSSL_DETECT_H

/**
 * Locates the OpenSSL shared library at runtime.
 *
 * @param out_path Output for the real path to libssl.so (caller must free).
 * @return A dlopen handle on success, NULL on failure.
 */
void* detect_openssl(const char** out_path);

/**
 * Captures the installed OpenSSL version string.
 *
 * Runs "openssl version" and extracts the version number.
 *
 * @return A malloc'd version string (caller must free).
 */
char* get_openssl_version(void);

#endif /* OPENSSL_OFFSET_OPENSSL_DETECT_H */
