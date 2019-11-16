// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <wasi/api.h>
#include <time.h>

const struct __clockid _CLOCK_THREAD_CPUTIME_ID = {
#ifdef __wasilibc_unmodified_upstream // generated constant names
    .id = __WASI_CLOCK_THREAD_CPUTIME_ID,
#else
    .id = __WASI_CLOCKID_THREAD_CPUTIME_ID,
#endif
};
