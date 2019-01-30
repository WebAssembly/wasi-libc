// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <wasi.h>
#include <time.h>

const struct __clockid _CLOCK_MONOTONIC = {
    .id = __WASI_CLOCK_MONOTONIC,
};
