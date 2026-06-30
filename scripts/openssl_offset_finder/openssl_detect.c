/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * detect the version of loaded OpenSSL library.
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openssl_detect.h"

void* detect_openssl(const char** out_path) {
  void* h = dlopen("libssl.so", RTLD_LAZY | RTLD_LOCAL);
  if (!h) return NULL;

  void* sym = dlsym(h, "OPENSSL_init_ssl");
  if (!sym) sym = dlsym(h, "BIO_f_ssl");
  if (sym) {
    Dl_info info;
    if (dladdr(sym, &info) && info.dli_fname) {
      char* real = realpath(info.dli_fname, NULL);
      if (real) {
        *out_path = real;
      } else {
        *out_path = strdup(info.dli_fname);
      }
    }
  }
  return h;
}

char* get_openssl_version(void) {
  FILE* fp = popen("openssl version 2>/dev/null", "r");
  if (!fp) return strdup("unknown");
  char buf[256];
  if (!fgets(buf, sizeof(buf), fp)) {
    pclose(fp);
    return strdup("unknown");
  }
  pclose(fp);
  char* ver = strdup("unknown");
  char* s = strstr(buf, "OpenSSL ");
  if (s) {
    s += 8;
    char* d = ver;
    while (*s && *s != ' ' && *s != '\n' && (d - ver) < 31)
      *d++ = *s++;
    *d = '\0';
  }
  return ver;
}
