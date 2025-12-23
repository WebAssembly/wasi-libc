// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <_/cdefs.h>
#include <stdnoreturn.h>
#include <unistd.h>

noreturn void _Exit(int status) {
#if defined(__wasip1__)
  __wasi_proc_exit(status);
#elif defined(__wasip2__)
  exit_result_void_void_t exit_status = { .is_err = status != 0 };
  exit_exit(&exit_status);
#else
# error "Unknown WASI version"
#endif
}

__strong_reference(_Exit, _exit);
