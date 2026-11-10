#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/libc-environ.h>
#include <wasi/libc.h>

/// If the program doesn't use `environ`, it'll get this version of
/// `__wasilibc_environ`, which isn't initialized with a constructor function.
/// `getenv` etc. call `__wasilibc_ensure_environ()` before accessing it.
/// Statically-initialize it to an invalid pointer value so that we can
/// detect if it's been explicitly initialized (we can't use `NULL` because
/// `clearenv` sets it to NULL.
weak char **__wasilibc_environ = (char **)-1;

// See the comments in libc-environ.h.
void __wasilibc_ensure_environ(void) {
  if (__wasilibc_environ == (char **)-1) {
    __wasilibc_initialize_environ();
  }
}

/// Avoid dynamic allocation for the case where there are no environment
/// variables, but we still need a non-NULL pointer to an (empty) array.
static char *empty_environ[1] = {NULL};

// Keep ownership separate from the environment array that callers can change.
static char **initial_environ;
#if defined(__wasip1__)
static char *initial_environ_buf;
#else
static void free_environ_strings(char **strings) {
  if (strings == NULL)
    return;
  for (char **p = strings; *p != NULL; ++p)
    free(*p);
  free(strings);
}
#endif

// See the comments in libc-environ.h.
void __wasilibc_initialize_environ(void) {
#if defined(__wasip1__)
  // Get the sizes of the arrays we'll have to create to copy in the
  // environment.
  size_t environ_count;
  size_t environ_buf_size;
  __wasi_errno_t err =
      __wasi_environ_sizes_get(&environ_count, &environ_buf_size);
  if (err != __WASI_ERRNO_SUCCESS) {
    goto oserr;
  }
  if (environ_count == 0) {
    __wasilibc_environ = empty_environ;
    return;
  }

  // Add 1 for the NULL pointer to mark the end, and check for overflow.
  size_t num_ptrs = environ_count + 1;
  if (num_ptrs == 0) {
    goto software;
  }

  // Allocate memory for storing the environment chars.
  char *environ_buf = malloc(environ_buf_size);
  if (environ_buf == NULL) {
    goto software;
  }

  // Allocate memory for the array of pointers. This uses `calloc` both to
  // handle overflow and to initialize the NULL pointer at the end.
  char **environ_ptrs = calloc(num_ptrs, sizeof(char *));
  if (environ_ptrs == NULL) {
    free(environ_buf);
    goto software;
  }

  // Fill the environment chars, and the `__wasilibc_environ` array with
  // pointers into those chars.
  // TODO: Remove the casts on `environ_ptrs` and `environ_buf` once the witx is
  // updated with char8 support.
  err = __wasi_environ_get((uint8_t **)environ_ptrs, (uint8_t *)environ_buf);
  if (err != __WASI_ERRNO_SUCCESS) {
    free(environ_buf);
    free(environ_ptrs);
    goto oserr;
  }

  initial_environ_buf = environ_buf;
  initial_environ = environ_ptrs;
  __wasilibc_environ = environ_ptrs;
  return;
oserr:
  _Exit(EX_OSERR);
software:
  _Exit(EX_SOFTWARE);
#elif defined(__wasip2__) || defined(__wasip3__)
#ifdef __wasip2__
  typedef wasip2_list_tuple2_string_string_t list_tuple2_string_string_t;
  typedef wasip2_tuple2_string_string_t tuple2_string_string_t;
#define list_tuple2_string_string_free wasip2_list_tuple2_string_string_free
#else
  typedef wasip3_list_tuple2_string_string_t list_tuple2_string_string_t;
  typedef wasip3_tuple2_string_string_t tuple2_string_string_t;
#define list_tuple2_string_string_free wasip3_list_tuple2_string_string_free
#endif
  // Get the environment
  list_tuple2_string_string_t wasi_environment;
  environment_get_environment(&wasi_environment);

  size_t environ_count = wasi_environment.len;
  if (environ_count == 0) {
    __wasilibc_environ = empty_environ;
    return;
  }

  char **strings = NULL;
  size_t num_ptrs = environ_count + 1;
  if (num_ptrs == 0)
    goto software;

  // Allocate both pointer arrays together. Keep the first one for cleanup.
  strings = calloc(num_ptrs, 2 * sizeof(char *));
  if (strings == NULL)
    goto software;
  char **environ_ptrs = strings + num_ptrs;

  for (size_t i = 0; i < environ_count; i++) {
    tuple2_string_string_t pair = wasi_environment.ptr[i];
    size_t size;
    if (__builtin_add_overflow(pair.f0.len, pair.f1.len, &size) ||
        __builtin_add_overflow(size, 2, &size))
      goto software;

    char *entry = malloc(size);
    if (entry == NULL)
      goto software;
    strings[i] = environ_ptrs[i] = entry;
    memcpy(entry, pair.f0.ptr, pair.f0.len);
    entry[pair.f0.len] = '=';
    memcpy(entry + pair.f0.len + 1, pair.f1.ptr, pair.f1.len);
    entry[size - 1] = '\0';
  }

  list_tuple2_string_string_free(&wasi_environment);
  initial_environ = strings;
  __wasilibc_environ = environ_ptrs;
  return;
software:
  free_environ_strings(strings);
  list_tuple2_string_string_free(&wasi_environment);
  _Exit(EX_SOFTWARE);
#elif defined(__webp2__)
  __wasilibc_environ = empty_environ;
#else
#error "Unsupported WASI version"
#endif
}

// See the comments in libc-environ.h.
void __wasilibc_deinitialize_environ(void) {
  if (__wasilibc_environ != (char **)-1) {
    // Release strings added by setenv before freeing the initial storage.
    clearenv();
#if defined(__wasip1__)
    free(initial_environ_buf);
    initial_environ_buf = NULL;
    free(initial_environ);
#else
    free_environ_strings(initial_environ);
#endif
    initial_environ = NULL;
    // Set the pointer to the special init value.
    __wasilibc_environ = (char **)-1;
  }
}

// See the comments in libc-environ.h.
weak void __wasilibc_maybe_reinitialize_environ_eagerly(void) {
  // This version does nothing. It may be overridden by a version which does
  // something if `environ` is used.
}
