// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <common/clock.h>
#include <assert.h>
#include <wasi/api.h>
#include <time.h>

#ifdef __wasip1__
static_assert(__WASI_CLOCKID_REALTIME == CLOCKID_REALTIME,
              "__WASI_CLOCKID_REALTIME has changed value");
#endif

const struct __clockid _CLOCK_REALTIME = {
    .id = CLOCKID_REALTIME,
};
