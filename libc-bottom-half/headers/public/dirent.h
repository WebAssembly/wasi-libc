#ifndef __wasilibc_dirent_h
#define __wasilibc_dirent_h

#include <cloudabi_types_common.h>

#define DT_BLK CLOUDABI_FILETYPE_BLOCK_DEVICE
#define DT_CHR CLOUDABI_FILETYPE_CHARACTER_DEVICE
#define DT_DIR CLOUDABI_FILETYPE_DIRECTORY
#define DT_FIFO CLOUDABI_FILETYPE_SOCKET_STREAM
#define DT_LNK CLOUDABI_FILETYPE_SYMBOLIC_LINK
#define DT_REG CLOUDABI_FILETYPE_REGULAR_FILE
#define DT_UNKNOWN CLOUDABI_FILETYPE_UNKNOWN

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
