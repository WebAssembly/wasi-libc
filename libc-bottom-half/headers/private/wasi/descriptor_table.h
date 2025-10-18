#ifndef DESCRIPTOR_TABLE_H
#define DESCRIPTOR_TABLE_H

#include <wasi/wasip2.h>

typedef struct {
        int dummy;
} tcp_socket_state_unbound_t;
typedef struct {
        int dummy;
} tcp_socket_state_bound_t;
typedef struct {
        int dummy;
} tcp_socket_state_connecting_t;
typedef struct {
        int dummy;
} tcp_socket_state_listening_t;

typedef struct {
        streams_own_input_stream_t input;
        poll_own_pollable_t input_pollable;
        streams_own_output_stream_t output;
        poll_own_pollable_t output_pollable;
} tcp_socket_state_connected_t;

typedef struct {
        network_error_code_t error_code;
} tcp_socket_state_connect_failed_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep the tag and union definitions in sync.
typedef struct {
        enum {
                TCP_SOCKET_STATE_UNBOUND,
                TCP_SOCKET_STATE_BOUND,
                TCP_SOCKET_STATE_CONNECTING,
                TCP_SOCKET_STATE_CONNECTED,
                TCP_SOCKET_STATE_CONNECT_FAILED,
                TCP_SOCKET_STATE_LISTENING,
        } tag;
        union {
                tcp_socket_state_unbound_t unbound;
                tcp_socket_state_bound_t bound;
                tcp_socket_state_connecting_t connecting;
                tcp_socket_state_connected_t connected;
                tcp_socket_state_connect_failed_t connect_failed;
                tcp_socket_state_listening_t listening;
        };
} tcp_socket_state_t;

typedef struct {
        tcp_own_tcp_socket_t socket;
        poll_own_pollable_t socket_pollable;
        bool blocking;
        bool fake_nodelay;
        bool fake_reuseaddr;
        network_ip_address_family_t family;
        tcp_socket_state_t state;
        monotonic_clock_duration_t send_timeout;
        monotonic_clock_duration_t recv_timeout;
} tcp_socket_t;

typedef struct {
        udp_own_incoming_datagram_stream_t incoming;
        poll_own_pollable_t incoming_pollable;
        udp_own_outgoing_datagram_stream_t outgoing;
        poll_own_pollable_t outgoing_pollable;
} udp_socket_streams_t;

typedef struct {
        int dummy;
} udp_socket_state_unbound_t;
typedef struct {
        int dummy;
} udp_socket_state_bound_nostreams_t;

typedef struct {
        udp_socket_streams_t streams; // Streams have no remote_address
} udp_socket_state_bound_streaming_t;

typedef struct {
        udp_socket_streams_t streams; // Streams have a remote_address
} udp_socket_state_connected_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep the tag and union definitions in sync.
// The "bound" state is split up into two distinct tags:
// - "bound_nostreams": Bound, but no datagram streams set up (yet). That will be done the first time send or recv is called.
// - "bound_streaming": Bound with active streams.
typedef struct {
        enum {
                UDP_SOCKET_STATE_UNBOUND,
                UDP_SOCKET_STATE_BOUND_NOSTREAMS,
                UDP_SOCKET_STATE_BOUND_STREAMING,
                UDP_SOCKET_STATE_CONNECTED,
        } tag;
        union {
                udp_socket_state_unbound_t unbound;
                udp_socket_state_bound_nostreams_t bound_nostreams;
                udp_socket_state_bound_streaming_t bound_streaming;
                udp_socket_state_connected_t connected;
        };
} udp_socket_state_t;

typedef struct {
        udp_own_udp_socket_t socket;
        poll_own_pollable_t socket_pollable;
        bool blocking;
        network_ip_address_family_t family;
        udp_socket_state_t state;
} udp_socket_t;

typedef enum read_directory_state_t {
        DIRECTORY_STATE_FILE,
        DIRECTORY_STATE_RETURNED_DOT,
        DIRECTORY_STATE_RETURNED_DOT_DOT
} read_directory_state_t;

typedef struct {
// Stream for contents of directory
        filesystem_own_directory_entry_stream_t directory_stream;
// File handle for the directory being listed
// (so that metadata hashes can be accessed).
        filesystem_borrow_descriptor_t directory_file_handle;
// State that reflects whether . and .. have been handled yet;
// this is used by readdir() since each call handles a single
// directory entry, and this state needs to be maintained across
// calls
        read_directory_state_t directory_state;
} directory_stream_entry_t;

// Stream representing an open file.
typedef struct {
// File was opened for reading
         bool readable;
// File was opened for writing
         bool writable;
         filesystem_borrow_descriptor_t file_handle;
} file_t;

typedef struct {
        streams_borrow_input_stream_t read_stream;
        streams_borrow_output_stream_t write_stream;
        // Current position in stream, relative to the beginning of the *file*, measured in bytes
        off_t offset;
        // Used for checking readiness to read/write to stream. Lazily initialized
        streams_own_pollable_t read_pollable;
        streams_own_pollable_t write_pollable;
        bool read_pollable_is_initialized;
        bool write_pollable_is_initialized;
        // When the stream is closed, the caller should
        // replace this entry in the table with the file handle
        file_t file_info;
} file_stream_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep the tag and union definitions in sync.
typedef struct {
        enum {
                DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET,
                DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET,
                DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE,
                DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM,
                DESCRIPTOR_TABLE_ENTRY_FILE_STREAM
        } tag;
        union {
                tcp_socket_t tcp_socket;
                udp_socket_t udp_socket;
                file_t file;
                directory_stream_entry_t directory_stream_info;
                file_stream_t stream;
        };
} descriptor_table_entry_t;

bool descriptor_table_update(int fd, descriptor_table_entry_t entry);

bool descriptor_table_insert(descriptor_table_entry_t entry, int *fd);

bool descriptor_table_get_ref(int fd, descriptor_table_entry_t **entry);

bool descriptor_table_remove(int fd, descriptor_table_entry_t *entry);

#endif
