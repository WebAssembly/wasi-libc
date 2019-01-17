// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>

#include <cloudabi_types.h>
#include <time.h>

const struct __clockid _CLOCK_MONOTONIC = {
    .id = CLOUDABI_CLOCK_MONOTONIC,
};
