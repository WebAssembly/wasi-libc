// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>
#include <time.h>

static_assert(TIMER_ABSTIME == __WASI_SUBSCRIPTION_CLOCK_ABSTIME,
              "Value mismatch");

#ifdef __wasilibc_unmodified_upstream
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    ...) {
#else
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    struct timespec *rmtp) {
#endif
  if ((flags & ~TIMER_ABSTIME) != 0)
    return EINVAL;

  // Prepare polling subscription.
  __wasi_subscription_t sub = {
      .type = __WASI_EVENTTYPE_CLOCK,
#ifdef __wasilibc_unmodified_upstream // non-anonymous unions
      .clock.clock_id = clock_id->id,
      .clock.flags = flags,
#else
      .u.clock.clock_id = clock_id->id,
      .u.clock.flags = flags,
#endif
  };
#ifdef __wasilibc_unmodified_upstream // non-anonymous unions
  if (!timespec_to_timestamp_clamp(rqtp, &sub.clock.timeout))
#else
  if (!timespec_to_timestamp_clamp(rqtp, &sub.u.clock.timeout))
#endif
    return EINVAL;

  // Block until polling event is triggered.
  size_t nevents;
  __wasi_event_t ev;
  __wasi_errno_t error = __wasi_poll_oneoff(&sub, &ev, 1, &nevents);
  return error == 0 && ev.error == 0 ? 0 : ENOTSUP;
}
