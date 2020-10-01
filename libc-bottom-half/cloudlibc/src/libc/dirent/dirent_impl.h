// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef DIRENT_DIRENT_IMPL_H
#define DIRENT_DIRENT_IMPL_H

#include <wasi/api.h>
#include <stddef.h>

struct dirent;

_Static_assert(sizeof(__wasi_dirent_t) == sizeof(struct dirent),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");
_Static_assert(offsetof(__wasi_dirent_t, d_next) == offsetof(struct dirent, d_loc),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");
_Static_assert(offsetof(__wasi_dirent_t, d_ino) == offsetof(struct dirent, d_ino),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");
_Static_assert(offsetof(__wasi_dirent_t, d_namlen) == offsetof(struct dirent, d_namlen),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");
_Static_assert(offsetof(__wasi_dirent_t, d_type) == offsetof(struct dirent, d_type),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");
_Static_assert(sizeof(__wasi_dirent_t) == offsetof(struct dirent, d_name),
	       "layout of `struct dirent` should match `__wasi_dirent_t`");

#define DIRENT_DEFAULT_BUFFER_SIZE 4096

struct _DIR {
  // Directory file descriptor and cookie.
  int fd;
  __wasi_dircookie_t cookie;

  // Read buffer.
  char *buffer;
  size_t buffer_processed;
  size_t buffer_size;
  size_t buffer_used;

  // Object returned by readdir().
  struct dirent *dirent;
  size_t dirent_size;

#ifdef __wasm32__
  // POSIX requires directory offsets to be represented as the C type `long`.
  // That's 32 bits on wasm32. But WASI uses 64-bit cookies. So we add a
  // layer of indirection.
  __wasi_dircookie_t *offsets;
  size_t offsets_len;
  size_t offsets_allocated;
#endif
};

#endif
