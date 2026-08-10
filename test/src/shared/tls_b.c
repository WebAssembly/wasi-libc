#include "tls_b.h"
#include "tls_a.h"

_Thread_local long long b_counter = B_COUNTER_INIT;

_Alignas(B_ALIGN) _Thread_local char b_aligned[B_ALIGN];

_Thread_local int b_blob[B_BLOB_SIZE];

long long b_counter_get(void) { return b_counter; }

void b_counter_set(long long value) { b_counter = value; }

void *b_counter_addr(void) { return &b_counter; }

void *b_aligned_addr(void) { return b_aligned; }

int b_blob_get(size_t index) { return b_blob[index]; }

void b_blob_set(size_t index, int value) { b_blob[index] = value; }

int b_read_a_counter(void) { return a_counter_get(); }
