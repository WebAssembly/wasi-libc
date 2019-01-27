#ifndef __wasilibc___header_sys_socket_h
#define __wasilibc___header_sys_socket_h

#include <__struct_msghdr.h>
#include <__struct_cmsghdr.h>
#include <__struct_sockaddr.h>
#include <__struct_sockaddr_storage.h>

#include <wasi.h>

#define SHUT_RD WASI_SHUT_RD
#define SHUT_WR WASI_SHUT_WR
#define SHUT_RDWR (SHUT_RD | SHUT_WR)

#define MSG_PEEK WASI_SOCK_RECV_PEEK
#define MSG_WAITALL WASI_SOCK_RECV_WAITALL
#define MSG_CTRUNC WASI_SOCK_RECV_FDS_TRUNCATED
#define MSG_TRUNC WASI_SOCK_RECV_DATA_TRUNCATED
#define MSG_PEEK WASI_SOCK_RECV_PEEK
#define MSG_WAITALL WASI_SOCK_RECV_WAITALL

#define SOCK_DGRAM WASI_FILETYPE_SOCKET_DGRAM
#define SOCK_STREAM WASI_FILETYPE_SOCKET_STREAM

#define SOCK_NONBLOCK (0x00004000)
#define SOCK_CLOEXEC (0x00002000)

#define SOL_SOCKET 0x7fffffff

#define SO_TYPE 3

#define SCM_RIGHTS 1

#define CMSG_DATA(cmsg) ((cmsg)->__cmsg_data)
#define CMSG_NXTHDR CMSG_NXTHDR
#define CMSG_FIRSTHDR CMSG_FIRSTHDR
#define CMSG_LEN(len) (__builtin_offsetof(struct cmsghdr, __cmsg_data[(len)]))
#define CMSG_SPACE(len) ((CMSG_LEN((len)) + (_Alignof(struct cmsghdr) - 1)) & \
                         (_Alignof(struct cmsghdr) - 1))

#define AF_UNSPEC 0
#define AF_INET 1
#define AF_INET6 2
#define AF_UNIX 3

#ifdef __cplusplus
extern "C" {
#endif

struct cmsghdr *CMSG_FIRSTHDR(const struct msghdr *);
struct cmsghdr *CMSG_NXTHDR(const struct msghdr *, const struct cmsghdr *);

#ifdef __cplusplus
}
#endif

#endif
