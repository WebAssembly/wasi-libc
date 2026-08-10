#ifndef WASI_LIBC_TEST_TLS_A_H
#define WASI_LIBC_TEST_TLS_A_H

#define A_COUNTER_INIT 0x0a0a0a0a
#define A_NAME_SIZE 32
#define A_NAME_INIT "tls_a"

int a_counter_get(void);
void a_counter_set(int value);
void *a_counter_addr(void);
const char *a_name_get(void);
void a_name_set(const char *value);
int a_zeroed_sum(void);
void a_zeroed_fill(int value);

#endif
