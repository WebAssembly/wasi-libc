#ifndef DESCRIPTOR_TABLE_H
#define DESCRIPTOR_TABLE_H

#include <reactor.h>

typedef enum {
    DESCRIPTOR_TABLE_VARIANT_TCP_NEW,
    DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTING,
    DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED,
    DESCRIPTOR_TABLE_VARIANT_TCP_ERROR,    
} descriptor_table_tag_t;

typedef struct {
    reactor_own_tcp_socket_t socket;
    bool blocking;
} descriptor_table_tcp_new_t;

typedef struct {
    descriptor_table_tcp_new_t socket;
    reactor_own_input_stream_t rx;
    reactor_own_output_stream_t tx;
} descriptor_table_tcp_connected_t;

typedef struct {
    descriptor_table_tcp_new_t socket;
    wasi_filesystem_0_2_0_rc_2023_10_18_types_error_code_t error_code;
} descriptor_table_tcp_error_t;

typedef union {
    descriptor_table_tcp_new_t tcp_new;
    descriptor_table_tcp_connected_t tcp_connected;
    descriptor_table_tcp_error_t tcp_error;
} descriptor_table_value_t;

typedef struct {
    descriptor_table_tag_t tag;
    descriptor_table_value_t value;
} descriptor_table_variant_t;

bool descriptor_table_insert(descriptor_table_variant_t variant, int* fd);

bool descriptor_table_update(int fd, descriptor_table_variant_t variant);

bool descriptor_table_get(int fd, descriptor_table_variant_t* variant);

bool descriptor_table_remove(int fd, descriptor_table_variant_t* variant);

#endif
