#include <unistd.h>
#include <errno.h>
#include <string.h>

extern char *__wasilibc_cwd;

char *getcwd(char *buf, size_t size)
{
  if (!buf) {
    buf = strdup(__wasilibc_cwd);
    if (!buf) {
      errno = ENOMEM;
      return -1;
    }
  } else {
    size_t len = strlen(__wasilibc_cwd);
    if (size < strlen(__wasilibc_cwd) + 1) {
      errno = ERANGE;
      return 0;
    }
    strcpy(buf, __wasilibc_cwd);
  }
  return buf;
}

