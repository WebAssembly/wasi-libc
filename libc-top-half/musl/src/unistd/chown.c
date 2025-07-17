#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"

int chown(const char *path, uid_t uid, gid_t gid)
{
#ifdef __wasilibc_unmodified_upstream /* WASIX has real no ownership yet */
#ifdef SYS_chown
	return syscall(SYS_chown, path, uid, gid);
#else
	return syscall(SYS_fchownat, AT_FDCWD, path, uid, gid, 0);
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
