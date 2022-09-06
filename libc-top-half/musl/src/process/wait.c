#ifdef __wasilibc_unmodified_upstream
#include <sys/wait.h>
#else
#include <unistd.h>
#endif

pid_t waitpid(pid_t pid, int *status, int options);

pid_t wait(int *status)
{
	return waitpid((pid_t)-1, status, 0);
}
