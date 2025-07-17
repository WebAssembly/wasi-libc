#include <unistd.h>
#include <errno.h>
#include "syscall.h"

int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flag)
{
#ifdef __wasilibc_unmodified_upstream /* WASIX has real no ownership yet */
	return syscall(SYS_fchownat, fd, path, uid, gid, flag);
#else
	// TODO: Implement ownership in WASIX
	if ((uid == 0 || uid == -1) && (gid == 0 || gid == -1))
		return 0; // No-op for WASIX, all files belong to 0:0
	// Every operation that would change ownership is not implemented in WASIX
	errno = ENOSYS;
	return -1;
#endif
}
