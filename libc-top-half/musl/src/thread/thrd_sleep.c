#include <threads.h>
#include <time.h>
#include <errno.h>

int thrd_sleep(const struct timespec *req, struct timespec *rem)
{
#ifdef __wasilibc_unmodified_upstream
	int ret = -__clock_nanosleep(CLOCK_REALTIME, 0, req, rem);
#else
	int ret = -nanosleep(req, rem);
#endif
	switch (ret) {
	case 0:      return 0;
	case -EINTR: return -1; /* value specified by C11 */
	default:     return -2;
	}
}