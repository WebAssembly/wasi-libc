#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define NUM_TCP_CLIENTS 4
#define TCP_BYTES 1024
#define TCP_CHUNK 64

#define NUM_UDP_CLIENTS 2
#define NUM_UDP_SERVERS 2
#define UDP_DATAGRAMS 16
#define UDP_PAYLOAD 64

#define NUM_FILE_WORKERS 2
#define FILE_ROUNDTRIPS 8
#define FILE_BYTES 1024
#define FILE_CHUNK 128

static _Atomic int tcp_bytes_verified;
static _Atomic int udp_datagrams_verified;
static _Atomic int file_roundtrips_done;

static unsigned char pattern(int id, int i) {
  return (unsigned char)(id * 37 + i * 11 + 5);
}

static void write_all(int fd, const unsigned char *buf, size_t len) {
  size_t off = 0;
  while (off < len) {
    ssize_t n = write(fd, buf + off, len - off);
    TEST(n > 0);
    off += n;
  }
}

// ===================== TCP echo =====================

static int tcp_listener;
static struct sockaddr_in tcp_addr;

static void *tcp_echo_handler(void *arg) {
  int conn = (int)(intptr_t)arg;
  unsigned char buf[TCP_CHUNK];
  while (1) {
    ssize_t n = read(conn, buf, sizeof(buf));
    TEST(n >= 0);
    if (n == 0)
      break;
    write_all(conn, buf, n);
  }
  TEST(close(conn) == 0);
  return NULL;
}

static void *tcp_acceptor(void *arg) {
  (void)arg;
  pthread_t handlers[NUM_TCP_CLIENTS];
  for (int i = 0; i < NUM_TCP_CLIENTS; i++) {
    int conn = accept(tcp_listener, NULL, NULL);
    TEST(conn >= 0);
    TEST(pthread_create(&handlers[i], NULL, tcp_echo_handler,
                        (void *)(intptr_t)conn) == 0);
  }
  for (int i = 0; i < NUM_TCP_CLIENTS; i++)
    TEST(pthread_join(handlers[i], NULL) == 0);
  TEST(close(tcp_listener) == 0);
  return NULL;
}

typedef struct {
  int fd;
  int id;
} tcp_client_t;

static void *tcp_client_reader(void *arg) {
  tcp_client_t *client = (tcp_client_t *)arg;
  unsigned char buf[TCP_CHUNK];
  int received = 0;
  while (received < TCP_BYTES) {
    ssize_t n = read(client->fd, buf, sizeof(buf));
    TEST(n > 0);
    if (n <= 0)
      break;
    for (ssize_t i = 0; i < n; i++) {
      if (buf[i] != pattern(client->id, received + i)) {
        t_error("tcp client %d: byte %d corrupted\n", client->id,
                received + (int)i);
        return NULL;
      }
    }
    received += n;
    tcp_bytes_verified += n;
  }
  return NULL;
}

static void *tcp_client(void *arg) {
  tcp_client_t *client = (tcp_client_t *)arg;
  client->fd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(client->fd >= 0);
  TEST(connect(client->fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) ==
       0);

  pthread_t reader;
  TEST(pthread_create(&reader, NULL, tcp_client_reader, client) == 0);

  unsigned char buf[TCP_CHUNK];
  for (int off = 0; off < TCP_BYTES; off += TCP_CHUNK) {
    for (int i = 0; i < TCP_CHUNK; i++)
      buf[i] = pattern(client->id, off + i);
    write_all(client->fd, buf, TCP_CHUNK);
  }

  TEST(pthread_join(reader, NULL) == 0);
  TEST(close(client->fd) == 0);
  return NULL;
}

// ===================== UDP echo =====================

static int udp_server_fd;
static struct sockaddr_in udp_server_addr;

static void *udp_echo_server(void *arg) {
  (void)arg;
  unsigned char buf[UDP_PAYLOAD];
  while (1) {
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t n = recvfrom(udp_server_fd, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from, &from_len);
    TEST(n > 0);
    if (n <= 0)
      return NULL;
    if (n == 4 && memcmp(buf, "QUIT", 4) == 0)
      return NULL;
    TEST(sendto(udp_server_fd, buf, n, 0, (struct sockaddr *)&from, from_len) ==
         n);
  }
}

static void *udp_client(void *arg) {
  int id = (int)(intptr_t)arg;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(fd >= 0);

  unsigned char out[UDP_PAYLOAD], in[UDP_PAYLOAD];
  for (int seq = 0; seq < UDP_DATAGRAMS; seq++) {
    out[0] = (unsigned char)seq;
    for (int i = 1; i < UDP_PAYLOAD; i++)
      out[i] = pattern(id + 100, seq * UDP_PAYLOAD + i);
    TEST(sendto(fd, out, sizeof(out), 0, (struct sockaddr *)&udp_server_addr,
                sizeof(udp_server_addr)) == sizeof(out));
    ssize_t n = recvfrom(fd, in, sizeof(in), 0, NULL, NULL);
    TEST(n == sizeof(out));
    if (memcmp(in, out, sizeof(out)) != 0) {
      t_error("udp client %d: datagram %d corrupted\n", id, seq);
      return NULL;
    }
    udp_datagrams_verified++;
  }
  TEST(close(fd) == 0);
  return NULL;
}

// ===================== File workers =====================

static void *file_worker(void *arg) {
  int id = (int)(intptr_t)arg;
  char name[64];
  unsigned char buf[FILE_CHUNK];

  for (int iter = 0; iter < FILE_ROUNDTRIPS; iter++) {
    snprintf(name, sizeof(name), "stress_%d_%d.bin", id, iter);
    int fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0644);
    TEST(fd >= 0);

    for (int off = 0; off < FILE_BYTES; off += FILE_CHUNK) {
      for (int i = 0; i < FILE_CHUNK; i++)
        buf[i] = pattern(id + 200, iter * FILE_BYTES + off + i);
      write_all(fd, buf, FILE_CHUNK);
    }

    TEST(lseek(fd, FILE_BYTES / 2, SEEK_SET) == FILE_BYTES / 2);
    TEST(read(fd, buf, FILE_CHUNK) == FILE_CHUNK);
    for (int i = 0; i < FILE_CHUNK; i++) {
      if (buf[i] != pattern(id + 200, iter * FILE_BYTES + FILE_BYTES / 2 + i)) {
        t_error("file worker %d: middle byte %d corrupted\n", id, i);
        return NULL;
      }
    }

    TEST(lseek(fd, 0, SEEK_SET) == 0);
    for (int off = 0; off < FILE_BYTES; off += FILE_CHUNK) {
      TEST(read(fd, buf, FILE_CHUNK) == FILE_CHUNK);
      for (int i = 0; i < FILE_CHUNK; i++) {
        if (buf[i] != pattern(id + 200, iter * FILE_BYTES + off + i)) {
          t_error("file worker %d: byte %d corrupted\n", id, off + i);
          return NULL;
        }
      }
    }

    TEST(close(fd) == 0);
    TEST(remove(name) == 0);
    file_roundtrips_done++;
  }
  return NULL;
}

// ===================== main =====================

int main(void) {
  // TCP echo server setup.
  tcp_listener = socket(AF_INET, SOCK_STREAM, 0);
  TEST(tcp_listener >= 0);
  socklen_t addr_len = sizeof(tcp_addr);
  memset(&tcp_addr, 0, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  tcp_addr.sin_port = 0;
  TEST(bind(tcp_listener, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) == 0);
  TEST(getsockname(tcp_listener, (struct sockaddr *)&tcp_addr, &addr_len) == 0);
  TEST(listen(tcp_listener, NUM_TCP_CLIENTS) == 0);

  // UDP echo server setup.
  udp_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(udp_server_fd >= 0);
  addr_len = sizeof(udp_server_addr);
  memset(&udp_server_addr, 0, sizeof(udp_server_addr));
  udp_server_addr.sin_family = AF_INET;
  udp_server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  udp_server_addr.sin_port = 0;
  TEST(bind(udp_server_fd, (struct sockaddr *)&udp_server_addr,
            sizeof(udp_server_addr)) == 0);
  TEST(getsockname(udp_server_fd, (struct sockaddr *)&udp_server_addr,
                   &addr_len) == 0);

  // Let a whole bunch of stuff run concurrently.
  pthread_t acceptor;
  TEST(pthread_create(&acceptor, NULL, tcp_acceptor, NULL) == 0);

  pthread_t udp_servers[NUM_UDP_SERVERS];
  for (int i = 0; i < NUM_UDP_SERVERS; i++)
    TEST(pthread_create(&udp_servers[i], NULL, udp_echo_server, NULL) == 0);

  tcp_client_t tcp_clients[NUM_TCP_CLIENTS];
  pthread_t tcp_client_threads[NUM_TCP_CLIENTS];
  for (int i = 0; i < NUM_TCP_CLIENTS; i++) {
    tcp_clients[i].id = i;
    TEST(pthread_create(&tcp_client_threads[i], NULL, tcp_client,
                        &tcp_clients[i]) == 0);
  }

  pthread_t udp_clients[NUM_UDP_CLIENTS];
  for (int i = 0; i < NUM_UDP_CLIENTS; i++)
    TEST(pthread_create(&udp_clients[i], NULL, udp_client,
                        (void *)(intptr_t)i) == 0);

  pthread_t file_workers[NUM_FILE_WORKERS];
  for (int i = 0; i < NUM_FILE_WORKERS; i++)
    TEST(pthread_create(&file_workers[i], NULL, file_worker,
                        (void *)(intptr_t)i) == 0);

  // Wait for everything to wind down.
  for (int i = 0; i < NUM_TCP_CLIENTS; i++)
    TEST(pthread_join(tcp_client_threads[i], NULL) == 0);
  for (int i = 0; i < NUM_UDP_CLIENTS; i++)
    TEST(pthread_join(udp_clients[i], NULL) == 0);
  for (int i = 0; i < NUM_FILE_WORKERS; i++)
    TEST(pthread_join(file_workers[i], NULL) == 0);
  TEST(pthread_join(acceptor, NULL) == 0);

  int quitter = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(quitter >= 0);
  for (int i = 0; i < NUM_UDP_SERVERS; i++)
    TEST(sendto(quitter, "QUIT", 4, 0, (struct sockaddr *)&udp_server_addr,
                sizeof(udp_server_addr)) == 4);
  for (int i = 0; i < NUM_UDP_SERVERS; i++)
    TEST(pthread_join(udp_servers[i], NULL) == 0);
  TEST(close(quitter) == 0);
  TEST(close(udp_server_fd) == 0);

  // Every byte everywhere arrived intact.
  TEST(tcp_bytes_verified == NUM_TCP_CLIENTS * TCP_BYTES);
  TEST(udp_datagrams_verified == NUM_UDP_CLIENTS * UDP_DATAGRAMS);
  TEST(file_roundtrips_done == NUM_FILE_WORKERS * FILE_ROUNDTRIPS);
  printf("tcp=%d bytes udp=%d datagrams files=%d roundtrips\n",
         tcp_bytes_verified, udp_datagrams_verified, file_roundtrips_done);

  return t_status;
}
