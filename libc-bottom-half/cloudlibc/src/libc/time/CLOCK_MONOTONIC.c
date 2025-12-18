// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <wasi/api.h>
#include <time.h>

#ifndef __wasilibc_use_wasip2
static_assert(__WASI_CLOCKID_MONOTONIC == CLOCKID_MONOTONIC,
              "__WASI_CLOCKID_MONOTONIC has changed value");
#endif

const struct __clockid _CLOCK_MONOTONIC = {
    .id = CLOCKID_MONOTONIC,
};
