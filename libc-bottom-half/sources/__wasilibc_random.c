#include <errno.h>
#include <unistd.h>
#include <wasi/version.h>

#ifndef __wasip1__
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <wasi/api.h>

#if defined(__wasip2__)
typedef wasip2_list_u8_t list_u8_t;
#define list_u8_free wasip2_list_u8_free
#elif defined(__wasip3__)
typedef wasip3_list_u8_t list_u8_t;
#define list_u8_free wasip3_list_u8_free
#else
#error "Unknown WASI version"
#endif

int __wasilibc_random(void *buffer, size_t len) {

  // Set up a WASI byte list to receive the results
  list_u8_t wasi_list;

  // Get random bytes
  random_get_random_bytes(len, &wasi_list);

  // The spec for get-random-bytes specifies that wasi_list.len
  // will be equal to len.
  if (wasi_list.len != len)
    _Exit(EX_OSERR);
  else {
    // Copy the result
    memcpy(buffer, wasi_list.ptr, len);
  }

  // Free the WASI byte list
  list_u8_free(&wasi_list);

  return 0;
}
#endif
