#ifdef __wasilibc_unmodified_upstream__
#else
#include <wasi.h>
#endif
#define _BSD_SOURCE
#include <unistd.h>
#include <sys/random.h>
#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
#include <pthread.h>
#endif
#include <errno.h>

int getentropy(void *buffer, size_t len)
{
	int cs, ret;
	char *pos = buffer;

	if (len > 256) {
		errno = EIO;
		return -1;
	}

#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
#endif

	while (len) {
#ifdef __wasilibc_unmodified_upstream__
		ret = getrandom(pos, len, 0);
#else
		errno = wasi_random_get(pos, len);
		if (errno == 0)
                    ret = len;
                else
                    ret = -1;
#endif
		if (ret < 0) {
			if (errno == EINTR) continue;
			else break;
		}
		pos += ret;
		len -= ret;
		ret = 0;
	}

#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
	pthread_setcancelstate(cs, 0);
#endif

	return ret;
}
