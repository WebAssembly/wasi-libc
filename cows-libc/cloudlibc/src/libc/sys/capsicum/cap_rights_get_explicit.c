// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/capsicum.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

static_assert(CAP_CREATE ==
                  (CLOUDABI_RIGHT_FILE_CREATE_FILE | CLOUDABI_RIGHT_FILE_OPEN),
              "Value mismatch");
static_assert(CAP_FDATASYNC == CLOUDABI_RIGHT_FD_DATASYNC, "Value mismatch");
static_assert(CAP_FEXECVE == CLOUDABI_RIGHT_PROC_EXEC, "Value mismatch");
static_assert(CAP_FSYNC == CLOUDABI_RIGHT_FD_SYNC, "Value mismatch");
static_assert(CAP_FTRUNCATE == CLOUDABI_RIGHT_FILE_STAT_FPUT_SIZE,
              "Value mismatch");
static_assert(CAP_MMAP == CLOUDABI_RIGHT_MEM_MAP, "Value mismatch");
static_assert(CAP_MMAP_R == (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_MEM_MAP),
              "Value mismatch");
static_assert(CAP_MMAP_RW == (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FD_WRITE |
                              CLOUDABI_RIGHT_MEM_MAP),
              "Value mismatch");
static_assert(CAP_MMAP_RWX ==
                  (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FD_WRITE |
                   CLOUDABI_RIGHT_MEM_MAP | CLOUDABI_RIGHT_MEM_MAP_EXEC),
              "Value mismatch");
static_assert(CAP_MMAP_RX == (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_MEM_MAP |
                              CLOUDABI_RIGHT_MEM_MAP_EXEC),
              "Value mismatch");
static_assert(CAP_MMAP_W == (CLOUDABI_RIGHT_FD_WRITE | CLOUDABI_RIGHT_MEM_MAP),
              "Value mismatch");
static_assert(CAP_MMAP_WX == (CLOUDABI_RIGHT_FD_WRITE | CLOUDABI_RIGHT_MEM_MAP |
                              CLOUDABI_RIGHT_MEM_MAP_EXEC),
              "Value mismatch");
static_assert(CAP_MMAP_X ==
                  (CLOUDABI_RIGHT_MEM_MAP | CLOUDABI_RIGHT_MEM_MAP_EXEC),
              "Value mismatch");
static_assert(CAP_POSIX_FADVISE == CLOUDABI_RIGHT_FILE_ADVISE,
              "Value mismatch");
static_assert(CAP_POSIX_FALLOCATE == CLOUDABI_RIGHT_FILE_ALLOCATE,
              "Value mismatch");
static_assert(CAP_PREAD == (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FD_SEEK),
              "Value mismatch");
static_assert(CAP_PWRITE == (CLOUDABI_RIGHT_FD_WRITE | CLOUDABI_RIGHT_FD_SEEK),
              "Value mismatch");
static_assert(CAP_READ == CLOUDABI_RIGHT_FD_READ, "Value mismatch");
static_assert(CAP_READDIR == CLOUDABI_RIGHT_FILE_READDIR, "Value mismatch");
static_assert(CAP_READLINKAT == CLOUDABI_RIGHT_FILE_READLINK, "Value mismatch");
static_assert(CAP_SEEK == (CLOUDABI_RIGHT_FD_SEEK | CLOUDABI_RIGHT_FD_TELL),
              "Value mismatch");
static_assert(CAP_SEEK_TELL == CLOUDABI_RIGHT_FD_TELL, "Value mismatch");
static_assert(CAP_WRITE == CLOUDABI_RIGHT_FD_WRITE, "Value mismatch");
static_assert(CAP_LOOKUP == CLOUDABI_RIGHT_FILE_OPEN, "Value mismatch");
static_assert(CAP_FCNTL == CLOUDABI_RIGHT_FD_STAT_PUT_FLAGS, "Value mismatch");
static_assert(CAP_FSTAT == CLOUDABI_RIGHT_FILE_STAT_FGET, "Value mismatch");
static_assert(CAP_FSTATAT == CLOUDABI_RIGHT_FILE_STAT_GET, "Value mismatch");
static_assert(CAP_FUTIMES == CLOUDABI_RIGHT_FILE_STAT_FPUT_TIMES,
              "Value mismatch");
static_assert(CAP_FUTIMESAT == CLOUDABI_RIGHT_FILE_STAT_PUT_TIMES,
              "Value mismatch");
static_assert(CAP_LINKAT_SOURCE == CLOUDABI_RIGHT_FILE_LINK_SOURCE,
              "Value mismatch");
static_assert(CAP_LINKAT_TARGET == CLOUDABI_RIGHT_FILE_LINK_TARGET,
              "Value mismatch");
static_assert(CAP_MKDIRAT == CLOUDABI_RIGHT_FILE_CREATE_DIRECTORY,
              "Value mismatch");
static_assert(CAP_RENAMEAT_SOURCE == CLOUDABI_RIGHT_FILE_RENAME_SOURCE,
              "Value mismatch");
static_assert(CAP_RENAMEAT_TARGET == CLOUDABI_RIGHT_FILE_RENAME_TARGET,
              "Value mismatch");
static_assert(CAP_SYMLINKAT == CLOUDABI_RIGHT_FILE_SYMLINK, "Value mismatch");
static_assert(CAP_UNLINKAT == CLOUDABI_RIGHT_FILE_UNLINK, "Value mismatch");
static_assert(CAP_RECV == CLOUDABI_RIGHT_FD_READ, "Value mismatch");
static_assert(CAP_SEND == CLOUDABI_RIGHT_FD_WRITE, "Value mismatch");
static_assert(CAP_SHUTDOWN == CLOUDABI_RIGHT_SOCK_SHUTDOWN, "Value mismatch");
static_assert(CAP_SOCK_CLIENT ==
                  (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FD_WRITE |
                   CLOUDABI_RIGHT_SOCK_SHUTDOWN),
              "Value mismatch");
static_assert(CAP_SOCK_SERVER ==
                  (CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FD_WRITE |
                   CLOUDABI_RIGHT_SOCK_SHUTDOWN),
              "Value mismatch");
static_assert(CAP_EVENT == CLOUDABI_RIGHT_POLL_FD_READWRITE, "Value mismatch");
static_assert(CAP_PDWAIT == CLOUDABI_RIGHT_POLL_PROC_TERMINATE,
              "Value mismatch");

int cap_rights_get_explicit(int fildes, cap_rights_t *base,
                            cap_rights_t *inheriting) {
  cloudabi_fdstat_t fsb;
  cloudabi_errno_t error = cloudabi_sys_fd_stat_get(fildes, &fsb);
  if (error != 0) {
    errno = error;
    return -1;
  }
  base->__value = fsb.fs_rights_base;
  inheriting->__value = fsb.fs_rights_inheriting;
  return 0;
}
