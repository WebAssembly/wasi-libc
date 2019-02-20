// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <arpa/inet.h>

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

static int inet_pton4(const char *restrict src, uint8_t *restrict dst) {
  uint8_t addr[4];
  for (size_t group = 0; group < __arraycount(addr); ++group) {
    // Parse number between 0 and 255.
    if (*src == '0') {
      // Zero.
      addr[group] = 0;
      ++src;
    } else if (*src >= '1' && *src <= '9') {
      // Number between 1 and 255.
      unsigned int number = *src++ - '0';
      if (*src >= '0' && *src <= '9')
        number = number * 10 + *src++ - '0';
      if (*src >= '0' && *src <= '9')
        number = number * 10 + *src++ - '0';
      if (number > 255)
        return 0;
      addr[group] = number;
    } else {
      // Number should consist of at least one digit.
      return 0;
    }

    // Require a trailing null byte and dot separator between groups.
    if (*src++ != (group == __arraycount(addr) - 1 ? '\0' : '.'))
      return 0;
  }

  // Copy result back.
  memcpy(dst, addr, sizeof(addr));
  return 1;
}

static int inet_pton6(const char *restrict src, uint8_t *restrict dst) {
#define NGROUPS 8
  uint8_t addr[NGROUPS * 2] = {};
  int leading_groups;
  int groups;

  // Handle leading "::".
  if (src[0] == ':' && src[1] == ':') {
    if (src[2] == '\0') {
      // The IPv6 null address.
      memset(dst, '\0', NGROUPS * 2);
      return 1;
    } else {
      // An address starting with "::", e.g. "::1".
      leading_groups = 0;
      groups = 2;
      src += 2;
    }
  } else {
    // Address should start with a group of numbers.
    leading_groups = NGROUPS;
    groups = 0;
  }

  // Parse and groups of hexadecimal digits between 0 and ffff.
  for (;;) {
    const char *group_start = src;
    uint_fast16_t number = 0;
    for (int i = 0; i < 4; ++i) {
      if (*src >= '0' && *src <= '9') {
        number = number * 16 + *src++ - '0';
      } else if (*src >= 'A' && *src <= 'F') {
        number = number * 16 + *src++ - 'A' + 10;
      } else if (*src >= 'a' && *src <= 'f') {
        number = number * 16 + *src++ - 'a' + 10;
      } else if (i == 0) {
        return 0;
      }
    }
    addr[groups * 2] = number >> 8;
    addr[groups * 2 + 1] = number;

    if (src[0] == ':' && src[1] == ':') {
      // "::" Can only be used once.
      ++groups;
      if (leading_groups < groups)
        return 0;
      leading_groups = groups;
      src += 2;
      if (*src == '\0') {
        // "::" placed at the end of an address.
        if (groups > NGROUPS - 2)
          return 0;
        break;
      } else {
        // "::" placed somewhere in the middle of an address.
        groups += 2;
      }
    } else if (*src == ':') {
      // Group separator.
      ++groups;
      ++src;
    } else if (*src == '\0') {
      // End of address.
      ++groups;
      break;
    } else {
      // Potential trailing IPv4 address using dotted quad notation.
      if (groups > NGROUPS - 2)
        return 0;
      if (inet_pton4(group_start, &addr[groups * 2]) != 1)
        return 0;
      groups += 2;
      break;
    }

    // Next iteration would attempt to parse a ninth group.
    if (groups >= NGROUPS)
      return 0;
  }

  // Number of groups is insufficient, e.g. "1:2:3:4:5:6:7".
  if (groups < leading_groups)
    return 0;

  // Zero the destination address. Copy the leading groups to the start
  // of the buffer and the trailing groups to the end.
  memset(dst, '\0', NGROUPS * 2);
  memcpy(dst, addr, leading_groups * 2);
  size_t trailing_groups = groups - leading_groups;
  memcpy(dst + (NGROUPS - trailing_groups) * 2, addr + leading_groups * 2,
         trailing_groups * 2);
#undef NGROUPS
  return 1;
}

int inet_pton(int af, const char *restrict src, void *restrict dst) {
  switch (af) {
    case AF_INET:
      return inet_pton4(src, dst);
    case AF_INET6:
      return inet_pton6(src, dst);
    default:
      errno = EAFNOSUPPORT;
      return -1;
  }
}
