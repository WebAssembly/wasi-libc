// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <arpa/inet.h>

#include <stdbool.h>
#include <stdint.h>

int inet_aton(const char *cp, struct in_addr *inp) {
  uint32_t max = UINT32_MAX;
  uint32_t leading = 0;
  int shift = 24;
  for (;;) {
    // Parse next part of the IPv4 address.
    typedef uint32_t int_t;
    uint32_t min = 0;
    int base = 0;
    bool allow_negative = false;
#define PEEK(n) cp[n]
#define SKIP(n) \
  do {          \
    cp += (n);  \
  } while (0)
#include <common/parser_strtoint.h>
#undef PEEK
#undef SKIP
    if (!have_number || have_overflow)
      return 0;

    if (*cp == '\0') {
      // End of string. Return the IPv4 address, combining the
      // previously parsed leading bytes with the trailing number.
      inp->s_addr = htonl(leading | number);
      return 1;
    } else if (shift > 0 && number <= UINT8_MAX && *cp++ == '.') {
      // More components follow.
      leading |= number << shift;
      shift -= 8;
      max >>= 8;
    } else {
      // Parse error.
      return 0;
    }
  }
}
