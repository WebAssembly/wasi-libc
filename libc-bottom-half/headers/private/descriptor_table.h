#ifndef DESCRIPTOR_TABLE_H
#define DESCRIPTOR_TABLE_H

#include <preview2.h>

typedef enum {
    TCP_SOCKET_STATE_UNBOUND,
    TCP_SOCKET_STATE_BOUND,
    TCP_SOCKET_STATE_CONNECTING,
    TCP_SOCKET_STATE_CONNECTED,
    TCP_SOCKET_STATE_CONNECT_FAILED,
    TCP_SOCKET_STATE_LISTENING,
} tcp_socket_state_tag_t;

typedef struct {} tcp_socket_state_unbound_t;
typedef struct {} tcp_socket_state_bound_t;
typedef struct {} tcp_socket_state_connecting_t;
typedef struct {} tcp_socket_state_listening_t;

typedef struct {
    streams_own_input_stream_t input;
    poll_own_pollable_t input_pollable;
    streams_own_output_stream_t output;
    poll_own_pollable_t output_pollable;
} tcp_socket_state_connected_t;

typedef struct {
    network_error_code_t error_code;
} tcp_socket_state_connect_failed_t;

typedef union {
    tcp_socket_state_unbound_t unbound;
    tcp_socket_state_bound_t bound;
    tcp_socket_state_connecting_t connecting;
    tcp_socket_state_connected_t connected;
    tcp_socket_state_connect_failed_t connect_failed;
    tcp_socket_state_listening_t listening;
} tcp_socket_state_t;

typedef struct {
    tcp_own_tcp_socket_t socket;
    poll_own_pollable_t socket_pollable;
    bool blocking;
    bool fake_nodelay;
    tcp_socket_state_tag_t state_tag;
    tcp_socket_state_t state;
} tcp_socket_t;




typedef struct {
    udp_own_udp_socket_t socket;
    bool blocking;
} udp_socket_t;


typedef enum {
    DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET,
    DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET,
} descriptor_table_entry_tag_t;

typedef union {
    tcp_socket_t tcp_socket;
    udp_socket_t udp_socket;
} descriptor_table_entry_value_t;

typedef struct {
    descriptor_table_entry_tag_t tag;
    descriptor_table_entry_value_t value;
} descriptor_table_entry_t;

bool descriptor_table_insert(descriptor_table_entry_t entry, int* fd);

bool descriptor_table_get_ref(int fd, descriptor_table_entry_t** entry);

bool descriptor_table_remove(int fd, descriptor_table_entry_t* entry);

#endif
