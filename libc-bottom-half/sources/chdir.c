#include "lock.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <lock.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/libc-find-relpath.h>
#include <wasi/libc.h>
#include <wasi/report-error.h>

#ifndef __webp2__
DECLARE_WEAK_LOCK(__wasilibc_cwd_lock, extern);
extern char *__wasilibc_cwd;
extern int __wasilibc_cwd_mallocd;
#endif

int chdir(const char *path) {
#if defined(__wasip1__) || defined(__wasip2__) || defined(__wasip3__)
  static char *relative_buf = NULL;
  static size_t relative_buf_len = 0;

  // Find a preopen'd directory as well as a relative path we're anchored
  // from which we're changing directories to.
  const char *abs;
  int parent_fd = __wasilibc_find_relpath_alloc(path, &abs, &relative_buf,
                                                &relative_buf_len, 1);
  if (parent_fd == -1)
    return -1;

  // Make sure that this directory we're accessing is indeed a directory.
  struct stat dirinfo;
  int ret = fstatat(parent_fd, relative_buf, &dirinfo, 0);
  if (ret == -1)
    return -1;
  if (!S_ISDIR(dirinfo.st_mode)) {
    errno = ENOTDIR;
    return -1;
  }

  // Create a string that looks like:
  //
  //    __wasilibc_cwd = "/" + abs + "/" + relative_buf
  //
  // If `relative_buf` is equal to "." or `abs` is equal to the empty string,
  // however, we skip that part and the middle slash.
  size_t abs_len = strlen(abs);
  int copy_relative = strcmp(relative_buf, ".") != 0;
  int mid = copy_relative && abs[0] != 0;
  char *new_cwd = malloc(1 + abs_len + mid +
                         (copy_relative ? strlen(relative_buf) : 0) + 1);
  if (new_cwd == NULL) {
    errno = ENOMEM;
    return -1;
  }
  new_cwd[0] = '/';
  strcpy(new_cwd + 1, abs);
  if (mid)
    new_cwd[1 + abs_len] = '/';
  if (copy_relative)
    strcpy(new_cwd + 1 + abs_len + mid, relative_buf);

  // And set our new malloc'd buffer into the global cwd, freeing the
  // previous one if necessary.
  WEAK_LOCK(__wasilibc_cwd_lock);
  char *prev_cwd = __wasilibc_cwd;
  __wasilibc_cwd = new_cwd;
  WEAK_UNLOCK(__wasilibc_cwd_lock);
  if (__wasilibc_cwd_mallocd)
    free(prev_cwd);
  __wasilibc_cwd_mallocd = 1;
  return 0;
#elif defined(__webp2__)
  (void)path;
  WASILIBC_LOG_UNSUPPORTED("chdir");
  errno = ENOENT;
  return -1;
#else
#error "Unknown WASI version"
#endif
}
