// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

#include "dirent_impl.h"

#ifdef __wasm32__
static int compare_cookies(const void *a, const void *b) {
    const __wasi_dircookie_t *da = (const __wasi_dircookie_t *)a;
    const __wasi_dircookie_t *db = (const __wasi_dircookie_t *)b;
    if (*da < *db) return -1;
    if (*da > *db) return 1;
    return 0;
}
#endif

long telldir(DIR *dirp) {
#ifdef __wasm32__
    const __wasi_dircookie_t *match =
        bsearch(&dirp->cookie,
                dirp->offsets, dirp->offsets_len, sizeof(__wasi_dircookie_t),
                compare_cookies);
    if (match != NULL)
        return (long)(match - dirp->offsets);

    // Since the POSIX-mandated return type is `long`, which is only 32 bits on
    // wasm32, store the actual offsets in a table and return an index.
    if (dirp->offsets_len == dirp->offsets_allocated) {
        size_t allocated = dirp->offsets_allocated;
        if (allocated == 0) {
            allocated = 4;
        } else if (__builtin_umull_overflow(allocated, 2, &allocated)) {
            goto enomem;
        }
        void *ptr =
            reallocarray(dirp->offsets, allocated, sizeof(__wasi_dircookie_t));
        if (ptr == NULL)
            goto enomem;
        dirp->offsets_allocated = allocated;
        dirp->offsets = ptr;
    }

    size_t index = dirp->offsets_len;
    size_t next_index = index + 1;
    if (next_index > (size_t)LONG_MAX)
        goto enomem;

    dirp->offsets[index] = dirp->cookie;
    dirp->offsets_len = next_index;
    qsort(dirp->offsets, dirp->offsets_len, sizeof(__wasi_dircookie_t),
          compare_cookies);
    return index;

enomem:
    errno = ENOMEM;
    return -1;
#else
    // On wasm64, we can just return the cookie directly.
    return dirp->cookie;
#endif
}
