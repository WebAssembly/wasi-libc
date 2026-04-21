#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <wasi/file_utils.h>
#include <wasi/wasip3_block.h>

/**
 * Validates that `ptr_signed` is a valid utf-8 string.
 *
 * Returns the byte-length of the string on success, or -1 if it's invalid
 * utf-8.
 */
static int validate_utf8(const char *ptr_signed) {
  const unsigned char *ptr = (const unsigned char *)ptr_signed;
  size_t i = 0;
  while (1) {
    unsigned char c = ptr[i];
    size_t codepoint_size;

    if (c == 0x00) {
      break;
    } else if (c <= 0x7F) {
      // fast path for ascii-like characters, just turn the loop.
      i++;
      continue;
    } else if ((c & 0xE0) == 0xC0) {
      codepoint_size = 2;
      // Disallow overlong encodings, which in this case is either 0xc0 or
      // 0xc1. All other patterns represent a valid encoding.
      if (c <= 0xC1)
        return -1;
    } else if ((c & 0xF0) == 0xE0) {
      codepoint_size = 3;
      // Disallow overlong encodings by ensuring that if no bits are set in
      // this byte then the highest payload bit in the next byte must be set.
      if (c == 0xE0 && (ptr[i + 1] & 0x20) == 0x00)
        return -1;

      // Disallow surrogates which start with 0xED and are followed with
      // something bigger than 0xA0.
      if (c == 0xED && ptr[i + 1] >= 0xA0)
        return -1;
    } else if ((c & 0xF8) == 0xF0) {
      codepoint_size = 4;

      // Disallow overlong encodings by ensuring that if no payload bits are
      // set in this byte then one of the upper two payload bits in the next
      // byte are set.
      if (c == 0xF0 && (ptr[i + 1] & 0x30) == 0x00)
        return -1;

      // Disallow codepoints beyond U+10FFFF which start with 0xF4 and are
      // followed with something bigger than 0x8F, or if the leading byte is
      // larger than 0xF4.
      if (c == 0xF4 && ptr[i + 1] >= 0x90)
        return -1;
      if (c > 0xF4)
        return -1;
    } else {
      return -1;
    }

    // Validate all continuation bytes are `0b10......`
    for (size_t j = 1; j < codepoint_size; j++)
      if ((ptr[i + j] & 0xC0) != 0x80)
        return -1;
    i += codepoint_size;
  }
  return i;
}

int wasi_string_from_c(const char *s, wasi_string_t *out) {
  int len = validate_utf8(s);
  if (len < 0) {
    errno = EILSEQ;
    return -1;
  }
  out->ptr = (uint8_t *)s;
  out->len = len;
  return 0;
}

#ifndef __wasip2__
/// Update `state` with the result of `code` that happened.
static size_t wasip3_io_update_code(wasip3_io_state_t *state,
                                    wasip3_waitable_status_t code) {
  assert(code != WASIP3_WAITABLE_STATUS_BLOCKED);
  switch (WASIP3_WAITABLE_STATE(code)) {
  case WASIP3_WAITABLE_COMPLETED:
  case WASIP3_WAITABLE_CANCELLED:
    break;
  case WASIP3_WAITABLE_DROPPED:
    state->flags |= WASIP3_IO_DONE;
    break;
  default:
    assert(0 && "unexpected waitable state");
    abort();
  }

  assert(state->flags & WASIP3_IO_INPROGRESS);
  state->flags &= ~WASIP3_IO_INPROGRESS;

  if (state->flags & WASIP3_IO_ZERO_INPROGRESS) {
    state->flags &= ~WASIP3_IO_ZERO_INPROGRESS;

    assert(!(state->flags & WASIP3_IO_SHOULD_BE_READY));
    state->flags |= WASIP3_IO_SHOULD_BE_READY;
  }

  return WASIP3_WAITABLE_COUNT(code);
}

/// Update `write` with the result of `event` that happened.
static size_t wasip3_io_update_event(wasip3_io_state_t *state,
                                     wasip3_event_t *event) {
  assert(event->event == WASIP3_EVENT_STREAM_WRITE ||
         event->event == WASIP3_EVENT_STREAM_READ);
  assert(event->waitable == state->stream);
  return wasip3_io_update_code(state, event->code);
}

/// Attempts to resolve any pending write that may be in-progress on `write`.
///
/// This may notably end up issuing more writes to finish a buffered write that
/// was previously flagged as completed.
static int wasip3_write_resolve_pending(wasi_write_t *write) {
  wasip3_event_t event;
  wasip3_io_state_t *state = write->state;

  // If there's nothing in-progress, then there's nothing to do, so bail out.
  if (!(state->flags & WASIP3_IO_INPROGRESS))
    return 0;
  assert(state->buf);

  // This loop ensures that the entirety of `write->buf` is written out. Libc
  // already reported that the write succeeded so if a short write happens then
  // it's got to get started back up again.
  while (1) {
    // For blocking I/O this awaits the previous result. For non-blocking I/O a
    // `poll` operation is done.
    if (write->blocking) {
      if (!__wasilibc_waitable_block_on(state->stream, &event,
                                        write->timeout)) {
        errno = ETIMEDOUT;
        return -1;
      }
    } else {
      __wasilibc_poll_waitable(state->stream, &event);
      if (event.event == WASIP3_EVENT_NONE) {
        errno = EWOULDBLOCK;
        return -1;
      }
    }

    // Update the I/O internal state given the result of the write.
    state->buf_start += wasip3_io_update_event(state, &event);

    // While there's remaining writes to perform, kick those off here. If a
    // write is blocked then nonblocking mode returns as such and blocking
    // mode breaks out to turn this outer `while (1)` loop again to block
    // on the result. If the write finishes immediately then state is updated
    // again and then further continues.
    while (!(state->flags & WASIP3_IO_DONE) &&
           state->buf_start != state->buf_end) {
      wasip3_waitable_status_t status = filesystem_stream_u8_write(
          state->stream, state->buf + state->buf_start,
          state->buf_end - state->buf_start);
      state->flags |= WASIP3_IO_INPROGRESS;
      if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
        if (write->blocking) {
          break;
        } else {
          errno = EWOULDBLOCK;
          return -1;
        }
      }
      state->buf_start += wasip3_io_update_code(state, status);
    }

    // We either broke out of the `while` loop normally, or we hit the `break`
    // in the loop which wants to continue to the top of this outer loop. Test
    // which it is here and act accordingly.
    if ((state->flags & WASIP3_IO_DONE) || state->buf_start == state->buf_end) {
      free(state->buf);
      state->buf = NULL;
      state->buf_start = 0;
      state->buf_end = 0;
      return 0;
    }
  }

  abort();
}

/// Helper of `read`/`write` starting below.
static bool wasip3_start_zero_length(wasip3_io_state_t *state,
                                     wasip3_waitable_status_t status) {
  assert(!(state->flags & WASIP3_IO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_ZERO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_SHOULD_BE_READY));
  assert(!(state->flags & WASIP3_IO_DONE));
  state->flags |= WASIP3_IO_INPROGRESS;
  state->flags |= WASIP3_IO_ZERO_INPROGRESS;
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED)
    return true;
  wasip3_io_update_code(state, status);
  return false;
}

/// Starts a zero-length write on the stream pointed to by `write`.
///
/// This requires that there's no active I/O on the stream at this time and
/// that it's not a closed stream.
static bool wasip3_write_start_zero_length(wasip3_io_state_t *state) {
  return wasip3_start_zero_length(
      state, filesystem_stream_u8_write(state->stream, NULL, 0));
}

/// Starts a zero-length read on the stream pointed to by `read`.
///
/// This requires that there's no active I/O on the stream at this time and
/// that it's not a closed stream.
static bool wasip3_read_start_zero_length(wasip3_io_state_t *state) {
  return wasip3_start_zero_length(
      state, filesystem_stream_u8_read(state->stream, NULL, 0));
}

/// Starts a zero-length read on the stream pointed to by `read`.
///
/// This requires that there's no active I/O on the stream at this time and
/// that it's not a closed stream.
static int wasip3_read_start_nonzero(wasip3_io_state_t *state, size_t length) {
  assert(state->flags & WASIP3_IO_MUST_BUFFER);
  assert(!(state->flags & WASIP3_IO_SHOULD_BE_READY));
  assert(!(state->flags & WASIP3_IO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_ZERO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_DONE));
  assert(state->buf == NULL);
  state->buf = malloc(length);
  if (!state->buf) {
    errno = ENOMEM;
    return -1;
  }
  state->buf_start = 0;
  state->buf_end = 0;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_read(state->stream, state->buf, length);
  state->flags |= WASIP3_IO_INPROGRESS;
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    errno = EWOULDBLOCK;
    return -1;
  }

  return 0;
}

static size_t wasip3_read_complete_internally(wasip3_io_state_t *state,
                                              void *buffer, size_t length) {
  size_t buf_len = state->buf_end - state->buf_start;
  size_t amount = buf_len < length ? buf_len : length;
  memcpy(buffer, state->buf + state->buf_start, amount);
  state->buf_start += amount;
  if (state->buf_start == state->buf_end) {
    free(state->buf);
    state->buf = NULL;
    state->buf_start = 0;
    state->buf_end = 0;
  }
  return amount;
}
#endif

ssize_t __wasilibc_write(wasi_write_t *write, const void *buffer,
                         size_t length) {
#if defined(__wasip2__)
  assert(write->output.__handle != 0);
  while (true) {
    streams_stream_error_t error;
    uint64_t count;

    // See how many bytes can be written to this stream, if any.
    if (!streams_method_output_stream_check_write(write->output, &count,
                                                  &error))
      return wasip2_handle_write_error(error);

    if (count) {
      // If bytes can be written to the stream, then attempt the write.
      count = count < length ? count : length;
      wasip2_list_u8_t list = {.ptr = (uint8_t *)buffer, .len = count};
      if (!streams_method_output_stream_write(write->output, &list, &error))
        return wasip2_handle_write_error(error);

      // For blocking writes additionally perform a blocking flush to ensure
      // that the data makes its way to the destination.
      if (write->blocking) {
        bool ok =
            streams_method_output_stream_blocking_flush(write->output, &error);
        if (!ok)
          return wasip2_handle_write_error(error);
      }

      // Update the offset if this stream is tracking that.
      if (write->offset)
        *write->offset += count;

      return count;
    }

    // This stream isn't currently writable, and this is a non-blocking
    // operation, so bail out.
    if (!write->blocking) {
      errno = EWOULDBLOCK;
      return -1;
    }

    // Lazily initialize the pollable if one hasn't already been created yet.
    if (write->pollable->__handle == 0)
      *write->pollable = streams_method_output_stream_subscribe(write->output);
    poll_borrow_pollable_t pollable_borrow =
        poll_borrow_pollable(*write->pollable);

    // Either wait for a timeout or indefinitely for this stream to become
    // writable. Once this is done loop around back to the beginning.
    if (write->timeout != 0) {
      monotonic_clock_own_pollable_t timeout_pollable =
          monotonic_clock_subscribe_duration(write->timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
#elif defined(__wasip3__)
  wasip3_io_state_t *state = write->state;

  // If this stream is closed, for example with a TCP shutdown, then it's
  // closed and we're at EOF.
  if (state->stream == 0)
    return write->eof(write->eof_data);

  // First resolve any pending I/O, should it exist.
  if (wasip3_write_resolve_pending(write) < 0)
    return -1;
  assert((state->flags & WASIP3_IO_INPROGRESS) == 0);
  assert((state->flags & WASIP3_IO_ZERO_INPROGRESS) == 0);

  // If this stream is complete, then delegate to EOF.
  if (state->flags & WASIP3_IO_DONE)
    return write->eof(write->eof_data);

  // For simplicity, handle blocking writes directly here. This involves
  // blocking with an optional timeout and handling the result.
  if (write->blocking) {
    state->flags &= ~WASIP3_IO_SHOULD_BE_READY;
    bool done = false;
    ssize_t amount = __wasilibc_stream_block_on_timeout(
        filesystem_stream_u8_write(state->stream, buffer, length),
        state->stream, &done, write->timeout,
        filesystem_stream_u8_cancel_write);
    if (done)
      state->flags |= WASIP3_IO_DONE;
    if (amount < 0)
      return -1;
    if (write->offset)
      *write->offset += amount;
    if (amount == 0 && done)
      return write->eof(write->eof_data);
    return amount;
  }

  // ... and everything below is about implementing nonblocking writes ...

  assert(!write->blocking);

  while (1) {
    bool should_be_ready = state->flags & WASIP3_IO_SHOULD_BE_READY;
    state->flags &= ~WASIP3_IO_SHOULD_BE_READY;

    // If this stream isn't forced to buffer then do an opportunistic write
    // with the user-provided buffer. If this doesn't work (it was blocked
    // instead of completing), but it should work (because a previous
    // zero-length operation succeeded which can sometimes mean it's ready),
    // then it means we must buffer from now on. No matter what though if the
    // write is in-progress then it's cancelled here as the user's input buffer
    // won't persist beyond this function call. Throughout writing/cancellation
    // if anything actually succeeded then that's immediately returned here.
    if (!(state->flags & WASIP3_IO_MUST_BUFFER)) {
      wasip3_waitable_status_t status =
          filesystem_stream_u8_write(state->stream, buffer, length);
      state->flags |= WASIP3_IO_INPROGRESS;
      if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
        if (should_be_ready)
          state->flags |= WASIP3_IO_MUST_BUFFER;
        status = filesystem_stream_u8_cancel_write(state->stream);
      }
      size_t amount = wasip3_io_update_code(state, status);
      if (amount > 0) {
        if (write->offset)
          *write->offset += amount;
        return amount;
      }
      if (state->flags & WASIP3_IO_DONE)
        return write->eof(write->eof_data);
    }

    // If, at this point, we're still not forced to buffer then it means that a
    // write was attempted above, cancelled, and nothing was transferred. In
    // that case perform a zero-length write as a readiness test for this
    // stream.
    if (!(state->flags & WASIP3_IO_MUST_BUFFER)) {
      if (wasip3_write_start_zero_length(state)) {
        errno = EWOULDBLOCK;
        return -1;
      }
      if (state->flags & WASIP3_IO_DONE)
        return write->eof(write->eof_data);

      // The zero-length write succeeded, so turn the loop again to retry the
      // write of the user-supplied buffer. Note that the I/O "should be ready"
      // flag will be set here which will poison the loop and avoid coming here
      // again if zero-length reads/writes don't actually work.
      assert(state->flags & WASIP3_IO_SHOULD_BE_READY);
      continue;
    }

    break;
  }

  // At this point we're in nonblocking mode, we're required to buffer data,
  // and most other flags should all be turned off. Here the input `bufffer` is
  // copied into the internal `state` and the I/O operation is issued.
  assert(state->flags & WASIP3_IO_MUST_BUFFER);
  assert(!(state->flags & WASIP3_IO_SHOULD_BE_READY));
  assert(!(state->flags & WASIP3_IO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_ZERO_INPROGRESS));
  assert(!(state->flags & WASIP3_IO_DONE));
  assert(state->buf == NULL);
  state->buf = malloc(length);
  if (!state->buf) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(state->buf, buffer, length);
  state->buf_start = 0;
  state->buf_end = length;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_write(state->stream, state->buf, length);
  state->flags |= WASIP3_IO_INPROGRESS;

  // If the I/O is blocked, then that's ok. The data is all owned by `state`
  // meaning that we've effectively just faked a write of `length` bytes. Here
  // it's reported as having written everything.
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    if (write->offset)
      *write->offset += length;
    return length;
  }

  // If that write actually succeeded, however, then deal with it here. That
  // means we have to deallocate the internal buffer and then handle the result
  // of the write.
  free(state->buf);
  state->buf = NULL;
  state->buf_start = 0;
  state->buf_end = 0;
  return wasip3_io_update_code(state, status);
#else
#error "Unknown WASI version"
#endif
}

ssize_t __wasilibc_read(wasi_read_t *read, void *buffer, size_t length) {
#if defined(__wasip2__)
  while (true) {
    wasip2_list_u8_t result;
    streams_stream_error_t error;

    // Attempt a read for the `length` we were passed in.
    if (!streams_method_input_stream_read(read->input, length, &result, &error))
      return wasip2_handle_read_error(error);

    // For empty reads after performing a successful 0-length read go ahead and
    // bail out.
    if (length == 0)
      return 0;

    // If bytes were read then copy those to the output `buffer`, deallocate
    // the list that the canonical ABI allocated, and return.
    if (result.len) {
      size_t len = result.len;
      memcpy(buffer, result.ptr, len);
      wasip2_list_u8_free(&result);
      if (read->offset)
        *read->offset += len;
      return len;
    }

    // Nonblocking reads bail out here as all that's left to do is block.
    if (!read->blocking) {
      errno = EWOULDBLOCK;
      return -1;
    }

    // Lazily initialize the pollable for this input stream.
    if (read->pollable->__handle == 0)
      *read->pollable = streams_method_input_stream_subscribe(read->input);
    poll_borrow_pollable_t pollable_borrow =
        poll_borrow_pollable(*read->pollable);

    // Wait either with a timeout or indefinitely for this read to complete.
    if (read->timeout != 0) {
      poll_own_pollable_t timeout_pollable =
          monotonic_clock_subscribe_duration(read->timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
#elif defined(__wasip3__)
  wasip3_io_state_t *state = read->state;
  wasip3_event_t event;

  // If this stream is closed, for example with a TCP shutdown, then it's
  // closed and we're at EOF.
  if (state->stream == 0)
    return read->eof(read->eof_data);

  // If there's active I/O in progress for this stream then this must wait for
  // it to complete.
  if (state->flags & WASIP3_IO_INPROGRESS) {
    if (read->blocking) {
      if (!__wasilibc_waitable_block_on(state->stream, &event, read->timeout)) {
        errno = ETIMEDOUT;
        return -1;
      }
    } else {
      __wasilibc_poll_waitable(state->stream, &event);
      if (event.event == WASIP3_EVENT_NONE) {
        errno = EWOULDBLOCK;
        return -1;
      }
    }
    // Upon completion that's the size of the read buffer which we're now
    // reading into.
    state->buf_end = wasip3_io_update_event(state, &event);
  }

  assert(!(state->flags & WASIP3_IO_INPROGRESS));

  // If this stream has internally buffered data that's ready to read then copy
  // that out here.
  if (state->buf)
    return wasip3_read_complete_internally(state, buffer, length);

  // If this stream has finished, then delegate to EOF.
  if (state->flags & WASIP3_IO_DONE)
    return read->eof(read->eof_data);

  // For simplicity handle the blocking read case here.
  if (read->blocking) {
    bool done = false;
    size_t amount = __wasilibc_stream_block_on_timeout(
        filesystem_stream_u8_read(read->state->stream, buffer, length),
        read->state->stream, &done, read->timeout,
        filesystem_stream_u8_cancel_read);
    if (done)
      state->flags |= WASIP3_IO_DONE;
    if (amount < 0)
      return -1;
    if (read->offset)
      *read->offset += amount;
    if (amount == 0 && done)
      return read->eof(read->eof_data);
    return amount;
  }

  // ... and everything below is about implementing nonblocking reads ...

  // NB: this loop structure is intentionally quite similar to `write`'s
  // structure above. See there for more comments.
  while (1) {
    bool should_be_ready = state->flags & WASIP3_IO_SHOULD_BE_READY;
    state->flags &= ~WASIP3_IO_SHOULD_BE_READY;

    if (!(state->flags & WASIP3_IO_MUST_BUFFER)) {
      wasip3_waitable_status_t status =
          filesystem_stream_u8_read(state->stream, buffer, length);
      state->flags |= WASIP3_IO_INPROGRESS;
      if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
        if (should_be_ready)
          state->flags |= WASIP3_IO_MUST_BUFFER;
        status = filesystem_stream_u8_cancel_read(state->stream);
      }
      size_t amount = wasip3_io_update_code(state, status);
      if (amount > 0) {
        if (read->offset)
          *read->offset += amount;
        return amount;
      }
      if (state->flags & WASIP3_IO_DONE)
        return read->eof(read->eof_data);
    }

    if (!(state->flags & WASIP3_IO_MUST_BUFFER)) {
      if (wasip3_read_start_zero_length(state)) {
        errno = EWOULDBLOCK;
        return -1;
      }
      if (state->flags & WASIP3_IO_DONE)
        return read->eof(read->eof_data);

      assert(state->flags & WASIP3_IO_SHOULD_BE_READY);
      continue;
    }

    break;
  }

  // Like with writes above this is the point where we're forced to do internal
  // buffering of reads. Issue the read here referencing internal data and
  // return that we're blocked.
  if (wasip3_read_start_nonzero(state, length) < 0)
    return -1;
  return wasip3_read_complete_internally(state, buffer, length);
#else
#error "Unknown WASI version"
#endif
}

#ifndef __wasip2__
static void wasip3_poll_read_ready(void *data, poll_state_t *state,
                                   wasip3_event_t *event) {
  wasip3_io_state_t *iostate = (wasip3_io_state_t *)data;
  iostate->buf_end = wasip3_io_update_event(iostate, event);
  __wasilibc_poll_ready(state, POLLRDNORM);
}

static void wasip3_poll_write_ready(void *data, poll_state_t *state,
                                    wasip3_event_t *event) {
  wasip3_io_state_t *iostate = (wasip3_io_state_t *)data;
  iostate->buf_start += wasip3_io_update_event(iostate, event);
  __wasilibc_poll_ready(state, POLLWRNORM);
}

static int wasip3_stream_poll(wasip3_io_state_t *iostate, poll_state_t *state,
                              short events, poll_ready_t ready) {
  // If the stream is closed then it's immediately ready for reading/writing as
  // that'll resolve with an error/0/etc.
  if (iostate->stream == 0) {
    __wasilibc_poll_ready(state, events);
    return 0;
  }

  // If the I/O stream is finished it'll never block so it's always ready.
  if (iostate->flags & WASIP3_IO_DONE) {
    __wasilibc_poll_ready(state, events);
    return 0;
  }

  // If there's I/O in progress, then that's what we're interested in so add it
  // to the set.
  if (iostate->flags & WASIP3_IO_INPROGRESS)
    return __wasilibc_poll_add(state, iostate->stream, ready, iostate);

  // For readable streams if there's buffered data then this is immediately
  // ready since that data can be read without blocking.
  if (events == POLLRDNORM && iostate->buf) {
    __wasilibc_poll_ready(state, events);
    return 0;
  }

  if (iostate->flags & WASIP3_IO_MUST_BUFFER) {
    // For writable streams, if buffering is required and zero-length writes
    // don't work then this is immediately ready since those writes never block
    // (they're buffered internally).
    if (events == POLLWRNORM) {
      __wasilibc_poll_ready(state, events);
      return 0;
    }

    // For readable streams if buffering is required then a read is kicked off
    // here. An arbitrary read size is chosen for now. If that read isn't ready
    // then the waitable is added, and otherwise it's flagged as immediately
    // ready.
    if (events == POLLRDNORM) {
      if (wasip3_read_start_nonzero(iostate, 16 * 1024) < 0) {
        if (errno == EWOULDBLOCK)
          return __wasilibc_poll_add(state, iostate->stream, ready, iostate);
        return -1;
      }
      __wasilibc_poll_ready(state, events);
      return 0;
    }
  }

  // If I/O should be ready then that means a zero-length op has already
  // completed, so this should be good to go.
  if (iostate->flags & WASIP3_IO_SHOULD_BE_READY) {
    __wasilibc_poll_ready(state, events);
    return 0;
  }

  // Kick off a zero-length write here. If that actually gets kicked off then
  // we're waiting on it so add it to the set. Otherwise it completed so we're
  // immediately ready.
  bool started_work = events == POLLWRNORM
                          ? wasip3_write_start_zero_length(iostate)
                          : wasip3_read_start_zero_length(iostate);
  if (started_work)
    return __wasilibc_poll_add(state, iostate->stream, ready, iostate);
  __wasilibc_poll_ready(state, events);
  return 0;
}

int __wasilibc_read_poll(wasip3_io_state_t *iostate, poll_state_t *state) {
  return wasip3_stream_poll(iostate, state, POLLRDNORM, wasip3_poll_read_ready);
}

int __wasilibc_write_poll(wasip3_io_state_t *iostate, poll_state_t *state) {
  return wasip3_stream_poll(iostate, state, POLLWRNORM,
                            wasip3_poll_write_ready);
}
#endif // !__wasip2__
