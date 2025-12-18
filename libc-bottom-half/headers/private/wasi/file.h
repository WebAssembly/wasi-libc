#ifndef WASI_FILE_H
#define WASI_FILE_H

#include <wasi/version.h>

#ifdef __wasip2__

#include <wasi/wasip2.h>

// Adds a new file handle to the descriptor table, returning the new `fd`.
int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag);

#endif // __wasip2__

#endif // WASI_FILE_H
