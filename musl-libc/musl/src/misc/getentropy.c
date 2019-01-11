#define _BSD_SOURCE
#include <unistd.h>
#include <sys/random.h>
#if defined(__wasm_musl_unmodified_upstream__) || !defined(__WASM_THREAD_MODEL_SINGLE)
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

#if defined(__wasm_musl_unmodified_upstream__) || !defined(__WASM_THREAD_MODEL_SINGLE)
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
#endif

	while (len) {
		ret = getrandom(pos, len, 0);
		if (ret < 0) {
			if (errno == EINTR) continue;
			else break;
		}
		pos += ret;
		len -= ret;
		ret = 0;
	}

#if defined(__wasm_musl_unmodified_upstream__) || !defined(__WASM_THREAD_MODEL_SINGLE)
	pthread_setcancelstate(cs, 0);
#endif

	return ret;
}
