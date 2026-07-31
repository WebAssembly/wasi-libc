#ifndef WASI_LIBC_TEST_TLS_NONE_H
#define WASI_LIBC_TEST_TLS_NONE_H

int none_add(int a, int b);
int none_state_get(void);
void none_state_set(int value);

#endif
