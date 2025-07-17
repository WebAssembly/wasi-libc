#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"

int fchown(int fd, uid_t uid, gid_t gid)
{
#ifdef __wasilibc_unmodified_upstream /* WASIX has real no ownership yet */
	int ret = __syscall(SYS_fchown, fd, uid, gid);
	if (ret != -EBADF || __syscall(SYS_fcntl, fd, F_GETFD) < 0)
		return __syscall_ret(ret);

	char buf[15+3*sizeof(int)];
	__procfdname(buf, fd);
#ifdef SYS_chown
	return syscall(SYS_chown, buf, uid, gid);
#else
	return syscall(SYS_fchownat, AT_FDCWD, buf, uid, gid, 0);
#endif
#else
	// TODO: Implement ownership in WASIX
	if ((uid == 0 || uid == -1) && (gid == 0 || gid == -1))
		return 0; // No-op for WASIX, all files belong to 0:0
	// Every operation that would change ownership is not implemented in WASIX
	errno = ENOSYS;
	return -1;
#endif

}
