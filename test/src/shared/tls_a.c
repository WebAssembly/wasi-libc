#include "tls_a.h"

#include <string.h>

_Thread_local int a_counter = A_COUNTER_INIT;
_Thread_local char a_name[A_NAME_SIZE] = A_NAME_INIT;
_Thread_local int a_zeroed[8];

int a_counter_get(void) { return a_counter; }

void a_counter_set(int value) { a_counter = value; }

void *a_counter_addr(void) { return &a_counter; }

const char *a_name_get(void) { return a_name; }

void a_name_set(const char *value) {
  strncpy(a_name, value, A_NAME_SIZE - 1);
  a_name[A_NAME_SIZE - 1] = '\0';
}

int a_zeroed_sum(void) {
  int sum = 0;
  for (size_t i = 0; i < sizeof(a_zeroed) / sizeof(a_zeroed[0]); i++)
    sum += a_zeroed[i];
  return sum;
}

void a_zeroed_fill(int value) {
  for (size_t i = 0; i < sizeof(a_zeroed) / sizeof(a_zeroed[0]); i++)
    a_zeroed[i] = value;
}
