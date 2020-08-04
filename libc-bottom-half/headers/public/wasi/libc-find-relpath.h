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
 * to be opened is stored in `relative_path`.
 *
 * Returns -1 if no directories were suitable or if an allocation error
 * happens.
 *
 * On success the `relative_path` points to a malloc'd string, so you'll
 * need to call `free` on it. The `abs_prefix` return does not need to be
 * free'd.
 */
int __wasilibc_find_relpath(const char *path,
                            const char **__restrict__ abs_prefix,
                            char **__restrict__ relative_path);

#ifdef __cplusplus
}
#endif

#endif
