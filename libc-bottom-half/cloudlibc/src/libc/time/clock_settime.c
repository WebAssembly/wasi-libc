
#include <common/clock.h>
#include <common/time.h>

#include <wasi/api.h>
#include <errno.h>
#include <time.h>

int __clock_settime(clockid_t clock_id, const struct timespec *tp) {
  __wasi_timestamp_t ts;
  if (!timespec_to_timestamp_exact(tp, &ts)) {
    errno = EINVAL;
    return -1;
  }
  __wasi_errno_t error = __wasi_clock_time_set(clock_id.id, ts);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
extern __typeof(__clock_settime) clock_settime __attribute__((__weak__, alias("__clock_settime")));
