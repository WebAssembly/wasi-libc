// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <wasi/api.h>
#include <time.h>

const struct __clockid _CLOCK_REALTIME = {
#ifdef __wasilibc_unmodified_upstream // generated constant names
    .id = __WASI_CLOCK_REALTIME,
#else
    .id = __WASI_CLOCKID_REALTIME,
#endif
};
