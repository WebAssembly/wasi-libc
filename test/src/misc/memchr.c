//! add-flags.py(LDFLAGS): -Wl,--stack-first -Wl,--initial-memory=327680

#include <__macro_PAGESIZE.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void test(char *ptr, size_t length, void *want) {
  void *got = memchr(ptr, 7, length);
  if (got != want) {
    printf("memchr(%p, 7, %lu) = %p, want %p\n", ptr, length, got, want);
  }
}

int main(void) {
  char *const LIMIT = (char *)(__builtin_wasm_memory_size(0) * PAGESIZE);

  for (ptrdiff_t length = 0; length < 64; length++) {
    for (ptrdiff_t alignment = 0; alignment < 24; alignment++) {
      for (ptrdiff_t pos = -2; pos < length + 2; pos++) {
        // Create a buffer with the given length, at a pointer with the given
        // alignment. Using the offset LIMIT - PAGESIZE - 8 means many buffers
        // will straddle a (Wasm, and likely OS) page boundary. Place the
        // character to find at every position in the buffer, including just
        // prior to it and after its end.
        char *ptr = LIMIT - PAGESIZE - 8 + alignment;
        memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
        memset(ptr, 5, length);

        // The first instance of the character is found.
        if (pos >= 0) ptr[pos + 2] = 7;
        ptr[pos] = 7;

        // The character is found if it's within range.
        test(ptr, length, 0 <= pos && pos < length ? &ptr[pos] : NULL);
      }
    }

    // Ensure we never read past the end of memory.
    char *ptr = LIMIT - length;
    memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
    memset(ptr, 5, length);
    ptr[length - 1] = 7;

    // Nothing found on an empty buffer.
    test(ptr, length, length != 0 ? &ptr[length - 1] : NULL);

    // Test for length overflow.
    if (length > 0) test(ptr, SIZE_MAX, &ptr[length - 1]);
  }

  return 0;
}
