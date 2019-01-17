// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/arc4random.h>

#include <cloudabi_syscalls.h>
#include <stdint.h>
#include <stdlib.h>
#include <threads.h>

static thread_local struct spritz arc4random_state = SPRITZ_INITIALIZER;
static thread_local size_t arc4random_bytesleft = 0;

void arc4random_buf(void *buf, size_t nbytes) {
  uint8_t *b = buf;
  while (nbytes-- > 0) {
    if (arc4random_bytesleft-- == 0) {
      // Call into the kernel to get more entropy.
      uint8_t key[128];
      if (cloudabi_sys_random_get(&key, sizeof(key)) != 0)
        abort();
      spritz_absorb(&arc4random_state, key, sizeof(key));

      // Force another stir after returning one megabyte of data.
      arc4random_bytesleft = 1 << 20;
    }
    *b++ = spritz_drip(&arc4random_state);
  }
}
