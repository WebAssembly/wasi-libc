#ifndef __wasi_libc_find_relpath_h
#define __wasi_libc_find_relpath_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A directory file descriptor plus a relative path within that directory.
 */
struct __wasilibc_relpath {
    /** File descriptor for the directory the path is relative to */
    int dirfd;

    /** The relative path from that directory */
    const char *relative_path;
};

/**
 * Look up the given path in the preopened directory map and return
 * a file descriptor and relative path that can be used to access it.
 * Ignore preopened directories which don't provide the specified
 * rights.
 *
 * Returns a `__wasilibc_relpath` with a `dirfd` of -1 if no preopened
 * directories were suitable.
 */
struct __wasilibc_relpath __wasilibc_find_relpath(const char *path,
                                                  __wasi_rights_t rights_base,
                                                  __wasi_rights_t rights_inheriting);

#ifdef __cplusplus
}
#endif

#endif
