#include "wasi/utf8.h"

#include <assert.h>
#include <string.h>

static bool utf8_replchar(wasilibc_utf8_writer_t *w) {
  if (w->raw_len + 3 > w->cap) {
    return false;
  }
  w->buf[w->raw_len++] = 0xEF;
  w->buf[w->raw_len++] = 0xBF;
  w->buf[w->raw_len++] = 0xBD;
  w->valid_len = w->raw_len;
  return true;
}

bool __wasilibc_utf8_write(wasilibc_utf8_writer_t *w, uint8_t b) {
  assert(w->valid_len <= w->raw_len);

  bool is_1byte = (b & 0x80) == 0;
  bool is_continuation = (b & 0xC0) == 0x80;
  bool is_2byte = (b & 0xE0) == 0xC0;
  bool is_3byte = (b & 0xF0) == 0xE0;
  bool is_4byte = (b & 0xF8) == 0xF0;

  if (!is_continuation && w->valid_len < w->raw_len) {
    // Unexpected non-continuation byte; rewind, emit replacement for the
    // aborted sequence, and continue with the current byte.
    if (!__wasilibc_utf8_abort_current(w)) {
      return false;
    }
  }

  if (is_1byte) {
    if (w->raw_len + 1 > w->cap) {
      return false;
    }
    w->buf[w->raw_len++] = b;
    w->valid_len = w->raw_len;
  } else if (is_2byte || is_3byte || is_4byte) {
    int32_t codepoint_length = is_2byte ? 2 : (is_3byte ? 3 : 4);
    if (w->raw_len + codepoint_length > w->cap) {
      return false;
    }
    w->codepoint_remaining = codepoint_length - 1;
    w->buf[w->raw_len++] = b;
    // Don't update valid_len
  } else if (is_continuation) {
    if (w->valid_len == w->raw_len) {
      // Unexpected continuation byte; emit a replacement char instead
      return utf8_replchar(w);
    } else {
      // Expected continuation byte
      w->buf[w->raw_len++] = b;
      w->codepoint_remaining -= 1;

      // If done, validate overlong / out-of-range
      if (w->codepoint_remaining <= 0) {
        uint8_t first = w->buf[w->valid_len];
        uint8_t second = w->buf[w->valid_len + 1];
        bool overlong = first == 0xC0 || first == 0xC1 ||
                        (first == 0xE0 && second < 0xA0) ||
                        (first == 0xF0 && second < 0x90);
        bool toobig = (first == 0xF4 && second >= 0x90) || 0xF5 <= first;
        bool surrogate = (first == 0xED && second >= 0xA0);
        if (overlong || toobig || surrogate) {
          // Rewind to start and write replacement character. If this fails,
          // restore all state to how it originally was.
          if (!__wasilibc_utf8_abort_current(w)) {
            w->raw_len--;
            w->codepoint_remaining += 1;
            return false;
          }
        }
        w->valid_len = w->raw_len;
      }
    }
  } else {
    return utf8_replchar(w);
  }

  return true;
}

bool __wasilibc_utf8_abort_current(wasilibc_utf8_writer_t *w) {
  assert(w->valid_len <= w->raw_len);
  if (w->valid_len == w->raw_len) {
    // No multi-byte code point in progress.
    return true;
  }
  if (w->valid_len + 3 > w->cap) {
    // No room for replacement character.
    return false;
  }
  w->raw_len = w->valid_len;
  utf8_replchar(w); // will succeed
  return true;
}

void __wasilibc_utf8_reset(wasilibc_utf8_writer_t *w) {
  size_t n = w->raw_len - w->valid_len;
  if (n > 0) {
    memmove(&w->buf[0], &w->buf[w->valid_len], n);
  }
  w->raw_len = n;
  w->valid_len = 0;
}
