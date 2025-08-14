//! add-flags.py(LDFLAGS): -Wl,--stack-first -Wl,--initial-memory=327680

#include <__macro_PAGESIZE.h>
#include <stdio.h>
#include <string.h>

void test(char *ptr, char *want) {
  char *got = strrchr(ptr, 7);
  if (got != want) {
    printf("strrchr(%p, 7) = %p, want %p\n", ptr, got, want);
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
        memset(ptr, 5, pos > length ? pos : length);

        // The last instance of the character is found.
        ptr[0] = 7;
        ptr[pos] = 7;
        ptr[length] = 0;

        // The character is found if it's within range.
        char *want = NULL;
        if (length > 0) want = 0 <= pos && pos < length ? &ptr[pos] : ptr;
        test(ptr, want);
      }
    }

    // We need space for the terminator.
    if (length <= 1) continue;

    // Ensure we never read past the end of memory.
    char *ptr = LIMIT - length;
    memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
    memset(ptr, 5, length);

    ptr[0] = 7;
    ptr[length - 2] = 7;
    ptr[length - 1] = 0;
    test(ptr, &ptr[length - 2]);
  }

  return 0;
}
