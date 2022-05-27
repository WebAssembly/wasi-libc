#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int setsockopt(int socket, int level, int option_name, const void *restrict option_value, socklen_t option_len) {
  if (level != SOL_SOCKET) {
    errno = ENOPROTOOPT;
    return -1;
  }

  switch (option_name) {
    case SO_ACCEPTCONN:
    case SO_BROADCAST:
    case SO_DONTROUTE:
    case SO_NODELAY:
    case SO_OOBINLINE:
    case SO_ONLYV6:
    case SO_REUSEPORT:
    case SO_REUSEADDR:
    case SO_MCASTLOOPV4:
    case SO_MCASTLOOPV6:
    case SO_KEEPALIVE: {
      __wasi_bool_t on = 0;
      if (option_len >= sizeof(int)) {
		 on = *((int*)&option_value) > 0 ? __WASI_BOOL_TRUE : __WASI_BOOL_FALSE;
	  } else {
		errno = EINVAL;
    	return -1;
	  }

      __wasi_errno_t error = __wasi_sock_set_opt(socket, option_name, on);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case SO_LINGER: {
      __wasi_option_timestamp_t tm;
	  if (option_len >= sizeof(struct linger)) {
		struct linger *linger = (struct linger *)option_value;
		tm.tag = linger->l_onoff > 0 ? __WASI_OPTION_SOME : __WASI_OPTION_NONE;
		tm.u.some = linger->l_linger * 1000000000ULL;
	  } else {
		errno = EINVAL;
    	return -1;
	  }
      __wasi_errno_t error = __wasi_sock_set_linger(socket, &tm);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case SO_RCVTIMEO:
    case SO_SNDTIMEO:
    case SO_CONNTIMEO:
    case SO_ACCPTIMEO: {
	  __wasi_option_timestamp_t tm;
	  if (option_len >= sizeof(struct timeval)) {
		struct timeval *tv = (struct timeval *)option_value;
		tm.tag = tv->tv_sec > 0 && tv->tv_usec > 0 ? __WASI_OPTION_SOME : __WASI_OPTION_NONE;
		tm.u.some = (tv->tv_sec * 1000000000ULL) + (tv->tv_usec * 1000ULL);
	  } else {
		errno = EINVAL;
    	return -1;
	  }

	  __wasi_timeout_type_t ty = 0;
      switch (option_name) {
        case SO_RCVTIMEO: { ty = __WASI_TIMEOUT_TYPE_READ; }
        case SO_SNDTIMEO: { ty = __WASI_TIMEOUT_TYPE_WRITE; }
        case SO_CONNTIMEO: { ty = __WASI_TIMEOUT_TYPE_CONNECT; }
        case SO_ACCPTIMEO: { ty = __WASI_TIMEOUT_TYPE_ACCEPT; }
      }

      __wasi_errno_t error = __wasi_sock_set_timeout(socket, ty, &tm);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case SO_RCVBUF: {
	  __wasi_filesize_t fs;
	  if (option_len >= sizeof(socklen_t)) {
		socklen_t *len = (socklen_t *)option_value;
		fs = *len;
	  } else {
		errno = EINVAL;
    	return -1;
	  }
      
      __wasi_errno_t error = __wasi_sock_set_recv_buf_size(socket, fs);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case SO_SNDBUF: {
      __wasi_filesize_t fs;
	  if (option_len >= sizeof(socklen_t)) {
		socklen_t *len = (socklen_t *)option_value;
		fs = *len;
	  } else {
		errno = EINVAL;
    	return -1;
	  }
      
      __wasi_errno_t error = __wasi_sock_set_send_buf_size(socket, fs);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
  }

  errno = ENOPROTOOPT;
  return -1;
}
