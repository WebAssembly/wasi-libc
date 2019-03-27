// Copyright (c) 2016-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_TLS_H
#define COMMON_TLS_H

#include <assert.h>
#include <wasi/core.h>
#include <stdalign.h>
#include <stddef.h>

#if defined(__aarch64__)

#define TLS_VARIANT 1
#define TCB_SIZE 16

// Fetches the TCB from the CPU's registers.
static inline __wasi_tcb_t *tcb_get(void) {
  __wasi_tcb_t *tcb;
  asm volatile("mrs %0, tpidr_el0" : "=r"(tcb));
  return tcb;
}

// Changes the TCB in the CPU's registers.
static inline void tcb_set(__wasi_tcb_t *tcb) {
  asm volatile("msr tpidr_el0, %0" : : "r"(tcb));
}

#elif defined(__arm__)

#define TLS_VARIANT 1
#define TCB_SIZE 8

// Fetches the TCB from the CPU's registers.
static inline __wasi_tcb_t *tcb_get(void) {
  __wasi_tcb_t *tcb;
  asm volatile("mrc p15, 0, %0, cr13, cr0, 2" : "=r"(tcb));
  return tcb;
}

// Changes the TCB in the CPU's registers.
static inline void tcb_set(__wasi_tcb_t *tcb) {
  asm volatile("mcr p15, 0, %0, cr13, cr0, 2" : : "r"(tcb));
}

#elif defined(__i386__)

#define TLS_VARIANT 2

// Fetches the TCB from the CPU's registers.
static inline __wasi_tcb_t *tcb_get(void) {
  __wasi_tcb_t *tcb;
  asm volatile("mov %%gs:0, %0" : "=r"(tcb));
  return tcb;
}

// Changes the TCB in the CPU's registers.
static inline void tcb_set(__wasi_tcb_t *tcb) {
  asm volatile("mov %0, %%gs:0" : : "r"(tcb));
}

#elif defined(__x86_64__)

#define TLS_VARIANT 2

// Fetches the TCB from the CPU's registers.
static inline __wasi_tcb_t *tcb_get(void) {
  __wasi_tcb_t *tcb;
  asm volatile("mov %%fs:0, %0" : "=r"(tcb));
  return tcb;
}

// Changes the TCB in the CPU's registers.
static inline void tcb_set(__wasi_tcb_t *tcb) {
  asm volatile("mov %0, %%fs:0" : : "r"(tcb));
}

#else
#error "Unsupported architecture"
#endif

#if TLS_VARIANT == 1

// TLS Variant I: TLS register points to the TCB. The TLS data is stored
// after the TCB. This approach has the disadvantage that the TCB size
// needs to be known.

static_assert(sizeof(__wasi_tcb_t) <= TCB_SIZE,
              "TCB does not fit in reserved space before TLS");

// Computes the total size needed to store a TCB with TLS data.
static inline size_t tls_size(void) {
  return TCB_SIZE + __pt_tls_memsz_aligned +
         (__pt_tls_align > alignof(__wasi_tcb_t) ? __pt_tls_align
                                                   : sizeof(__wasi_tcb_t)) -
         1;
}

// Computes the address of the TCB in the combined TCB/TLS area.
static inline __wasi_tcb_t *tcb_addr(char *buf) {
  if (alignof(__wasi_tcb_t) < __pt_tls_align) {
    return (
        __wasi_tcb_t *)(__roundup((uintptr_t)buf + TCB_SIZE, __pt_tls_align) -
                          TCB_SIZE);
  } else {
    return (__wasi_tcb_t *)__roundup((uintptr_t)buf, alignof(__wasi_tcb_t));
  }
}

// Computes the address of the TLS data in the combined TCB/TLS area.
static inline char *tls_addr(char *buf) {
  return (char *)tcb_addr(buf) + TCB_SIZE;
}

// Fetches the TLS area of the currently running thread.
static inline char *tls_get(void) {
  return (char *)tcb_get() + TCB_SIZE;
}

#elif TLS_VARIANT == 2

// TLS Variant II: TLS register points to the TCB. The TLS data is
// stored before the TCB. This approach has the advantage that the TCB
// size does not need to be known.

// Computes the total size needed to store a TCB with TLS data.
static inline size_t tls_size(void) {
  return __pt_tls_memsz_aligned + sizeof(__wasi_tcb_t) +
         (__pt_tls_align > alignof(__wasi_tcb_t) ? __pt_tls_align
                                                   : sizeof(__wasi_tcb_t)) -
         1;
}

// Computes the address of the TLS data in the combined TCB/TLS area.
static inline char *tls_addr(char *buf) {
  if (alignof(__wasi_tcb_t) < __pt_tls_align) {
    return (char *)(__roundup((uintptr_t)buf, __pt_tls_align));
  } else {
    return (char *)(__roundup((uintptr_t)buf + __pt_tls_memsz_aligned,
                              alignof(__wasi_tcb_t)) -
                    __pt_tls_memsz_aligned);
  }
}

// Computes the address of the TCB in the combined TCB/TLS area.
static inline __wasi_tcb_t *tcb_addr(char *buf) {
  return (__wasi_tcb_t *)(tls_addr(buf) + __pt_tls_memsz_aligned);
}

// Fetches the TLS area of the currently running thread.
static inline char *tls_get(void) {
  return (char *)tcb_get() - __pt_tls_memsz_aligned;
}

#else
#error "Unknown TLS variant"
#endif

// Changes the CPU's registers to point to a new TLS area.
//
// This function ensures that the TCB of the old TLS area is copied into
// the new TLS area. This ensures that the runtime (kernel, emulator,
// etc) still has access to its own private data.
static inline void tls_replace(char *buf) {
  __wasi_tcb_t *tcb = tcb_addr(buf);
  *tcb = *tcb_get();
  tcb_set(tcb);
}

#endif
