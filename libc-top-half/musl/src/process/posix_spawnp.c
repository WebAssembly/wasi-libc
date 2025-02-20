#include <spawn.h>
#include <unistd.h>

int __posix_spawn(pid_t *restrict res, const char *restrict path,
				  const posix_spawn_file_actions_t *fa,
				  const posix_spawnattr_t *restrict attr,
				  char *const argv[restrict], char *const envp[restrict],
				  uint8_t use_path);

int posix_spawnp(pid_t *restrict res, const char *restrict file,
	const posix_spawn_file_actions_t *fa,
	const posix_spawnattr_t *restrict attr,
	char *const argv[restrict], char *const envp[restrict])
{
#if defined(__wasilibc_unmodified_upstream) || defined(__wasilibc_fork_based_posix_spawn)
	posix_spawnattr_t spawnp_attr = { 0 };
	if (attr) spawnp_attr = *attr;
	spawnp_attr.__fn = (void *)__execvpe;	
	return posix_spawn(res, file, fa, &spawnp_attr, argv, envp);
#else
	return __posix_spawn(res, file, fa, attr, argv, envp, 1);
#endif
}
