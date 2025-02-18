#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <spawn.h>
#include "stdio_impl.h"
#include "syscall.h"

#ifndef __wasilibc_unmodified_upstream
#define __environ __wasilibc_environ
#endif
extern char **__environ;

FILE *popen(const char *cmd, const char *mode)
{
	int p[2], op, e;
	pid_t pid;
	FILE *f;
	posix_spawn_file_actions_t fa;

#ifndef __wasilibc_unmodified_upstream
	__wasilibc_ensure_environ();
#endif

	if (*mode == 'r') {
		op = 0;
	} else if (*mode == 'w') {
		op = 1;
	} else {
		errno = EINVAL;
		return 0;
	}
	
	if (pipe2(p, O_CLOEXEC)) return NULL;
	f = fdopen(p[op], mode);
	if (!f) {
#ifdef __wasilibc_unmodified_upstream
		__syscall(SYS_close, p[0]);
		__syscall(SYS_close, p[1]);
#else
		close(p[0]);
		close(p[1]);
#endif
		return NULL;
	}
	FLOCK(f);

	/* If the child's end of the pipe happens to already be on the final
	 * fd number to which it will be assigned (either 0 or 1), it must
	 * be moved to a different fd. Otherwise, there is no safe way to
	 * remove the close-on-exec flag in the child without also creating
	 * a file descriptor leak race condition in the parent. */
	if (p[1-op] == 1-op) {
		int tmp = fcntl(1-op, F_DUPFD_CLOEXEC, 0);
		if (tmp < 0) {
			e = errno;
			goto fail;
		}
#ifdef __wasilibc_unmodified_upstream
		__syscall(SYS_close, p[1-op]);
#else
		close(p[1-op]);
#endif
		p[1-op] = tmp;
	}

	e = ENOMEM;
	if (!posix_spawn_file_actions_init(&fa)) {
		if (!posix_spawn_file_actions_adddup2(&fa, p[1-op], 1-op)) {
			if (!(e = posix_spawn(&pid, "/bin/sh", &fa, 0,
			    (char *[]){ "sh", "-c", (char *)cmd, 0 }, __environ))) {
				posix_spawn_file_actions_destroy(&fa);
				f->pipe_pid = pid;
				if (!strchr(mode, 'e'))
					fcntl(p[op], F_SETFD, 0);
#ifdef __wasilibc_unmodified_upstream
				__syscall(SYS_close, p[1-op]);
#else
				close(p[1-op]);
#endif
				FUNLOCK(f);
				return f;
			}
		}
		posix_spawn_file_actions_destroy(&fa);
	}
fail:
	fclose(f);
#ifdef __wasilibc_unmodified_upstream
	__syscall(SYS_close, p[1-op]);
#else
	close(p[1-op]);
#endif

	errno = e;
	return 0;
}
