// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <fcntl.h>

static_assert(POSIX_FADV_DONTNEED == __WASI_ADVICE_DONTNEED,
              "Value mismatch");
static_assert(POSIX_FADV_NOREUSE == __WASI_ADVICE_NOREUSE, "Value mismatch");
static_assert(POSIX_FADV_NORMAL == __WASI_ADVICE_NORMAL, "Value mismatch");
static_assert(POSIX_FADV_RANDOM == __WASI_ADVICE_RANDOM, "Value mismatch");
static_assert(POSIX_FADV_SEQUENTIAL == __WASI_ADVICE_SEQUENTIAL,
              "Value mismatch");
static_assert(POSIX_FADV_WILLNEED == __WASI_ADVICE_WILLNEED,
              "Value mismatch");

int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
  if (offset < 0 || len < 0)
    return EINVAL;

#ifdef __wasilibc_use_wasip2
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return EBADF;
  }

  filesystem_error_code_t error_code;
  filesystem_advice_t fs_advice;
  switch (advice) {
  case POSIX_FADV_NORMAL:
    fs_advice = FILESYSTEM_ADVICE_NORMAL;
    break;
  case POSIX_FADV_SEQUENTIAL:
    fs_advice = FILESYSTEM_ADVICE_SEQUENTIAL;
    break;
  case POSIX_FADV_RANDOM:
    fs_advice = FILESYSTEM_ADVICE_RANDOM;
    break;
  case POSIX_FADV_NOREUSE:
    fs_advice = FILESYSTEM_ADVICE_NO_REUSE;
    break;
  case POSIX_FADV_WILLNEED:
    fs_advice = FILESYSTEM_ADVICE_WILL_NEED;
    break;
  case POSIX_FADV_DONTNEED:
    fs_advice = FILESYSTEM_ADVICE_DONT_NEED;
    break;
  default: {
    errno = EINVAL;
    return EINVAL;
  }
  }
  bool ok = filesystem_method_descriptor_advise(file_handle, offset, len, fs_advice, &error_code);
  if (!ok) {
    translate_error(error_code);
    return errno;
  }
  return 0;
#else
  return __wasi_fd_advise(fd, offset, len, advice);
#endif
}
