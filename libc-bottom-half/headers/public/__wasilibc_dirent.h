#ifndef __wasilibc___wasilibc_dirent_h
#define __wasilibc___wasilibc_dirent_h

#include <wasi.h>

#define DT_BLK WASI_FILETYPE_BLOCK_DEVICE
#define DT_CHR WASI_FILETYPE_CHARACTER_DEVICE
#define DT_DIR WASI_FILETYPE_DIRECTORY
#define DT_FIFO WASI_FILETYPE_SOCKET_STREAM
#define DT_LNK WASI_FILETYPE_SYMBOLIC_LINK
#define DT_REG WASI_FILETYPE_REGULAR_FILE
#define DT_UNKNOWN WASI_FILETYPE_UNKNOWN

#include <__struct_dirent.h>
#include <__typedef_DIR.h>

#ifdef __cplusplus
extern "C" {
#endif

int closedir(DIR *);
DIR *opendir(const char *);
DIR *fdopendir(int);
int fdclosedir(DIR *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);
void seekdir(DIR *, long);
long telldir(DIR *);
DIR *opendirat(int, const char *);
void rewinddir(DIR *);
int scandirat(int, const char *, struct dirent ***,
              int (*)(const struct dirent *),
              int (*)(const struct dirent **, const struct dirent **));

#ifdef __cplusplus
}
#endif

#endif
