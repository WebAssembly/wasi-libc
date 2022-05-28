#include <sys/socket.h>
#include <__header_netinet_in.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int setsockopt(int socket, int level, int option_name, const void *restrict option_value, socklen_t option_len) {
  if (level == IPPROTO_IPV6 && option_name == IPV6_V6ONLY) {
    level = SOL_SOCKET;
    option_name = SO_ONLYV6;
  }
  if (level == IPPROTO_TCP && option_name == TCP_NODELAY) {
    level = SOL_SOCKET;
    option_name = SO_NODELAY;
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

      __wasi_errno_t error = __wasi_sock_set_opt_flag(socket, option_name, on);
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
      __wasi_errno_t error = __wasi_sock_set_opt_time(socket, option_name, &tm);
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

	  __wasi_errno_t error = __wasi_sock_set_opt_time(socket, option_name, &tm);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case SO_RCVBUF:
	case SO_SNDBUF:
	case SO_TTL:
	case SO_MCASTTTLV4: {
	  __wasi_filesize_t fs;
	  if (option_len >= sizeof(socklen_t)) {
		socklen_t *len = (socklen_t *)option_value;
		fs = *len;
	  } else {
		errno = EINVAL;
    	return -1;
	  }
      
      __wasi_errno_t error = __wasi_sock_set_opt_size(socket, option_name, fs);
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
