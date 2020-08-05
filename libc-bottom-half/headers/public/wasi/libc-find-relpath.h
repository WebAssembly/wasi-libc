#ifndef __wasi_libc_find_relpath_h
#define __wasi_libc_find_relpath_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Look up the given `path`, relative to the cwd, in the preopened directory
 * map. If a suitable entry is found, then the file descriptor for that entry
 * is returned. Additionally the absolute path of the directory's file
 * descriptor is returned in `abs_prefix` and the relative portion that needs
 * to be opened is stored in `relative_path`. The size of the `relative_path`
 * buffer is provided in the `relative_path_len` argument.
 *
 * Returns -1 on failure. Errno is set to either:
 *
 *  * ENOMEM - failed to allocate memory for internal routines.
 *  * ENOENT - the `path` could not be found relative to any preopened dir.
 *  * ERANGE - the `relative_path` buffer is too small to hold the relative path.
 */
int __wasilibc_find_relpath(const char *path,
                            const char **__restrict__ abs_prefix,
                            char *relative_path,
                            size_t relative_path_len);

#ifdef __cplusplus
}
#endif

#endif
