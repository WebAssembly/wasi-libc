// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <wasi/api.h>
#include <time.h>

const struct __clockid _CLOCK_PROCESS_CPUTIME_ID = {
#ifdef __wasilibc_unmodified_upstream
    .id = __WASI_CLOCK_PROCESS_CPUTIME_ID,
#else
    .id = __WASI_CLOCKID_PROCESS_CPUTIME_ID,
#endif
};
