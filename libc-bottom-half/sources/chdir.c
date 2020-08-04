#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/libc-find-relpath.h>
#include <wasi/libc.h>

char *__wasilibc_cwd = "/";
static int __wasilibc_cwd_mallocd = 0;

int chdir(const char *path)
{
  // Find a preopen'd directory as well as a relative path we're anchored
  // from which we're changing directories to.
  char *relative;
  const char *abs;
  int parent_fd = __wasilibc_find_relpath(path, &abs, &relative);
  if (parent_fd == -1) {
    errno = ENOENT;
    return -1;
  }

  // Make sure that this directory we're accessing is indeed a directory.
  struct stat dirinfo;
  int ret = fstatat(parent_fd, relative, &dirinfo, 0);
  if (ret == -1) {
    free(relative);
    return -1;
  }
  if (!S_ISDIR(dirinfo.st_mode)) {
    free(relative);
    errno = ENOTDIR;
    return -1;
  }

  // Copy over our new absolute path into `__wasilibc_cwd`. Only copy over
  // the relative portion of the path if it's not equal to `.`
  size_t len = strlen(abs);
  int copy_relative = strcmp(relative, ".") != 0;
  char *new_cwd = malloc(len + (copy_relative ? strlen(relative) : 0));
  if (new_cwd == NULL) {
    errno = ENOMEM;
    return -1;
  }
  strcpy(new_cwd, abs);
  if (copy_relative)
    strcpy(new_cwd + strlen(abs), relative);
  free(relative);

  // And set our new malloc'd buffer into the global cwd, freeing the
  // previous one if necessary.
  char *prev_cwd = __wasilibc_cwd;
  __wasilibc_cwd = new_cwd;
  if (__wasilibc_cwd_mallocd)
    free(prev_cwd);
  __wasilibc_cwd_mallocd = 1;
  return 0;
}
