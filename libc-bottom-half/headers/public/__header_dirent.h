#ifndef __wasilibc___header_dirent_h
#define __wasilibc___header_dirent_h

#include <wasi/api.h>

#define DT_UNKNOWN 0
#define DT_BLK 1
#define DT_CHR 2
#define DT_DIR 3
#define DT_REG 4
#define DT_FIFO 6
#define DT_LNK 7
#define DT_SOCK 20

#define IFTODT(x) (__wasilibc_iftodt(x))
#define DTTOIF(x) (__wasilibc_dttoif(x))

#include <__struct_dirent.h>
#include <__typedef_DIR.h>

#ifdef __cplusplus
extern "C" {
#endif

int __wasilibc_iftodt(int x);
int __wasilibc_dttoif(int x);

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
