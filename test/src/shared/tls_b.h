#ifndef WASI_LIBC_TEST_TLS_B_H
#define WASI_LIBC_TEST_TLS_B_H

#include <stddef.h>

#define B_COUNTER_INIT 0x0b0b0b0b
#define B_ALIGN 64
#define B_BLOB_SIZE 24

long long b_counter_get(void);
void b_counter_set(long long value);
void *b_counter_addr(void);
void *b_aligned_addr(void);
int b_blob_get(size_t index);
void b_blob_set(size_t index, int value);
int b_read_a_counter(void);

#endif
