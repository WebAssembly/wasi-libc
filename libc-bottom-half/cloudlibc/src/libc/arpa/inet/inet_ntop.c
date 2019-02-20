// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static const char *inet_ntop_inet(const uint8_t *restrict src,
                                  char *restrict dst, size_t size) {
  // Format the address.
  char buf[INET_ADDRSTRLEN];
  size_t len =
      snprintf(buf, sizeof(buf), "%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8,
               src[0], src[1], src[2], src[3]);

  // Copy it back.
  if (len >= size) {
    errno = ENOSPC;
    return NULL;
  }
  strlcpy(dst, buf, size);
  return dst;
}

static const char *inet_ntop_inet6(const struct in6_addr *restrict src,
                                   char *restrict dst, size_t size) {
  // Extract groups from address.
  uint16_t groups[8];
  for (size_t i = 0; i < __arraycount(groups); ++i)
    groups[i] = (uint16_t)src->s6_addr[i * 2] << 8 | src->s6_addr[i * 2 + 1];

  // Find longest series of groups having value zero.
  struct {
    size_t start;
    size_t len;
  } zeroes_cur = {}, zeroes_best = {};
  for (size_t i = 0; i < __arraycount(groups); ++i) {
    if (groups[i] == 0) {
      if (zeroes_best.len < ++zeroes_cur.len)
        zeroes_best = zeroes_cur;
    } else {
      zeroes_cur.start = i + 1;
      zeroes_cur.len = 0;
    }
  }

  // Format the address.
  char buf[INET6_ADDRSTRLEN];
  char *bufend = buf;
  size_t i = 0;
  int strip_colon = 1;
  bool ipv4 = IN6_IS_ADDR_V4COMPAT(src) || IN6_IS_ADDR_V4MAPPED(src);
  do {
    size_t bufsize = buf + sizeof(buf) - bufend;
    if (i == 6 && ipv4) {
      // End address with IPv4 representation of the last four bytes.
      bufend +=
          snprintf(bufend, bufsize,
                   &":%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8[strip_colon],
                   src->s6_addr[12], src->s6_addr[13], src->s6_addr[14],
                   src->s6_addr[15]);
      break;
    } else if (i == zeroes_best.start && zeroes_best.len > 1) {
      *bufend++ = ':';
      *bufend++ = ':';
      i += zeroes_best.len;
      strip_colon = 1;
    } else {
      bufend += snprintf(bufend, bufsize, &":%" PRIx16[strip_colon], groups[i]);
      ++i;
      strip_colon = 0;
    }
  } while (i < __arraycount(groups));
  *bufend++ = '\0';

  // Copy it back.
  size_t len = bufend - buf;
  if (len > size) {
    errno = ENOSPC;
    return NULL;
  }
  memcpy(dst, buf, len);
  return dst;
}

const char *inet_ntop(int af, const void *restrict src, char *restrict dst,
#ifdef __wasilibc_unmodified_upstream // bug fix
                      size_t size) {
#else
                      socklen_t size) {
#endif
  switch (af) {
    case AF_INET:
      return inet_ntop_inet(src, dst, size);
    case AF_INET6: {
      struct in6_addr v6addr;
      memcpy(&v6addr, src, sizeof(v6addr));
      return inet_ntop_inet6(&v6addr, dst, size);
    }
    default:
      errno = EAFNOSUPPORT;
      return NULL;
  }
}
