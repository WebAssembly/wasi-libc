#include <unistd.h>
#include <errno.h>

#ifndef __wasilibc_unmodified_upstream
extern int __wasilibc_pgrp;
#endif

pid_t setpgrp(void)
{
#ifdef __wasilibc_unmodified_upstream
	return setpgid(0, 0);
#else
  errno = ENOSYS;
  return __wasilibc_pgrp;
#endif
}
