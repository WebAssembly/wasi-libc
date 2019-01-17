// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <time.h>

static_assert(TIMER_ABSTIME == CLOUDABI_SUBSCRIPTION_CLOCK_ABSTIME,
              "Value mismatch");

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    ...) {
  if ((flags & ~TIMER_ABSTIME) != 0)
    return EINVAL;

  // Prepare polling subscription.
  cloudabi_subscription_t sub = {
      .type = CLOUDABI_EVENTTYPE_CLOCK,
      .clock.clock_id = clock_id->id,
      .clock.flags = flags,
  };
  if (!timespec_to_timestamp_clamp(rqtp, &sub.clock.timeout))
    return EINVAL;

  // Block until polling event is triggered.
  size_t nevents;
  cloudabi_event_t ev;
  cloudabi_errno_t error = cloudabi_sys_poll(&sub, &ev, 1, &nevents);
  return error == 0 && ev.error == 0 ? 0 : ENOTSUP;
}
