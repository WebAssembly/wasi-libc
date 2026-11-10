#include <stdlib.h>
#include <string.h>
#include <wasi/libc-environ.h>

#include "test.h"

extern char **environ;

static void check_environment(void) {
  const char *value = getenv("WASI_ENV_TEST");
  const char *empty = getenv("WASI_ENV_EMPTY");
  if (value == NULL || strcmp(value, "original") != 0 || empty == NULL ||
      *empty != '\0')
    t_error("expected WASI_ENV_TEST=original and an empty WASI_ENV_EMPTY\n");
}

int main(void) {
  char borrowed[] = "borrowed=value";
  check_environment();
  if (setenv("WASI_ENV_TEST", "changed", 1) != 0 || putenv(borrowed) != 0) {
    t_error("cannot update the environment\n");
    return t_status;
  }
  __wasilibc_deinitialize_environ();
  check_environment();

  char *replacement[] = {borrowed, NULL};
  clearenv();
  environ = replacement;
  __wasilibc_deinitialize_environ();
  check_environment();
  if (strcmp(borrowed, "borrowed=value") != 0 || replacement[0] != borrowed ||
      replacement[1] != NULL)
    t_error("environment reset changed caller-owned storage\n");
  return t_status;
}
