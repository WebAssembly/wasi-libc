#ifndef WASI_WASIP3_BLOCK_H
#define WASI_WASIP3_BLOCK_H

#include <wasi/api.h>

#ifdef __wasip3__
#include <stdint.h>
#include <wasi/descriptor_table.h> // for waitable_t

// Waits for a subtask to return
void __wasilibc_subtask_block_on_and_drop(wasip3_subtask_t status);

/// Waits for `future` to be resolved after a previous operation yielded
/// `status` as a result.
///
/// This function will block if `status` indicates that the future is blocked,
/// and this function won't return until the future has become ready again. This is
/// suitable, for example, for waiting for future reads to complete.
void __wasilibc_future_block_on(wasip3_subtask_status_t status, uint32_t future);

/// Waits for `stream` to be resolved after a previous operation yielded
/// `status` as a result.
///
/// This function will block if `status` indicates that the stream is blocked,
/// and this function won't return until the stream has become ready again. This is
/// suitable, for example, for waiting for stream read or write to complete.
///
/// The `closed` variable is set based on the result of the operation to
/// communicate what was received from the component model.
size_t __wasilibc_stream_block_on(wasip3_subtask_status_t status, uint32_t stream, bool *closed);
#endif

#endif // WASI_WASIP3_BLOCK_H
