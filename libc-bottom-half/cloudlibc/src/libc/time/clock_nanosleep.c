// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <time.h>

#ifndef __wasilibc_use_wasip2
static_assert(TIMER_ABSTIME == __WASI_SUBCLOCKFLAGS_SUBSCRIPTION_CLOCK_ABSTIME,
              "Value mismatch");
#endif

#ifdef __wasilibc_use_wasip2
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    struct timespec *rmtp) {
  if ((flags & ~TIMER_ABSTIME) != 0)
    return EINVAL;

  // Note: rmtp is ignored

  if (clock_id != CLOCK_MONOTONIC) {
    // wasip2 only provides a pollable for monotonic clocks
    return ENOTSUP;
  }

  // Prepare pollable
  int64_t duration = (rqtp->tv_sec * NSEC_PER_SEC) + rqtp->tv_nsec;
  monotonic_clock_own_pollable_t pollable;
  if (flags & TIMER_ABSTIME) {
    pollable = monotonic_clock_subscribe_instant(duration);
  } else {
    pollable = monotonic_clock_subscribe_duration(duration);
  }

  // Block until polling event is triggered.
  poll_method_pollable_block(poll_borrow_pollable(pollable));

  poll_pollable_drop_own(pollable);
  return 0;
}
#else
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    struct timespec *rmtp) {
  if ((flags & ~TIMER_ABSTIME) != 0)
    return EINVAL;

  // Prepare polling subscription.
  __wasi_subscription_t sub = {
      .u.tag = __WASI_EVENTTYPE_CLOCK,
      .u.u.clock.id = clock_id->id,
      .u.u.clock.flags = flags,
  };
  if (!timespec_to_timestamp_clamp(rqtp, &sub.u.u.clock.timeout))
    return EINVAL;

  // Block until polling event is triggered.
  size_t nevents;
  __wasi_event_t ev;
  __wasi_errno_t error = __wasi_poll_oneoff(&sub, &ev, 1, &nevents);
  return error == 0 && ev.error == 0 ? 0 : ENOTSUP;
}
#endif

weak_alias(clock_nanosleep, __clock_nanosleep);
