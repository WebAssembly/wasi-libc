// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <sys/resource.h>

#include <wasi/api.h>
#include <errno.h>

int getrusage(int who, struct rusage *r_usage) {
  switch (who) {
    case RUSAGE_SELF: {
      __wasi_timestamp_t usertime = 0;
      (void)__wasi_clock_time_get(__WASI_CLOCKID_PROCESS_CPUTIME_ID, 1000,
                                  &usertime);
      *r_usage = (struct rusage){
          .ru_utime = timestamp_to_timeval(usertime),
      };
      return 0;
    }
    case RUSAGE_CHILDREN:
      *r_usage = (struct rusage){};
      return 0;
    default:
      errno = EINVAL;
      return -1;
  }
}
