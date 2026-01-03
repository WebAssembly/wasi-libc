#include <wasi/version.h>

#ifdef __wasip3__
#include <stdlib.h>
#include <wasi/descriptor_table.h>
#include <wasi/api.h>
#include <common/errors.h>

typedef struct {
    filesystem_stream_u8_writer_t handle;
} pipe3w_t;

typedef struct {
    filesystem_stream_u8_t handle;
} pipe3r_t;

static int pipe_read_stream(void *data, filesystem_stream_u8_t *out,
                            off_t **offs) {
  pipe3r_t *file = (pipe3r_t *)data;
  *out = file->handle;
  *offs = 0;
  return 0;
}

static void pipe_r_free(void *data) {
  pipe3r_t *file = (pipe3r_t *)data;
  filesystem_stream_u8_drop_readable(file->handle);
  free(file);
}

static int pipe_write_stream(void *data, filesystem_stream_u8_t *out,
                            off_t **offs) {
  pipe3w_t *file = (pipe3w_t *)data;
  *out = file->handle;
  *offs = 0;
  return 0;
}

static void pipe_w_free(void *data) {
  pipe3w_t *file = (pipe3w_t *)data;
  filesystem_stream_u8_drop_writable(file->handle);
  free(file);
}

static descriptor_vtable_t pipe_r_vtable = {
    .free = pipe_r_free,
    .get_read_stream3 = pipe_read_stream,
};

static descriptor_vtable_t pipe_w_vtable = {
    .free = pipe_w_free,
    .get_write_stream3 = pipe_write_stream,
};

int pipe(int pipefd[2]) {
  pipe3w_t *writehandle = calloc(1, sizeof(pipe3w_t));
  if (!writehandle) {
    errno = ENOMEM;
    return -1;
  }
  pipe3r_t *readhandle = calloc(1, sizeof(pipe3r_t));
  if (!readhandle) {
    free(writehandle);
    errno = ENOMEM;
    return -1;
  }
  readhandle->handle = filesystem_stream_u8_new(&writehandle->handle);
  descriptor_table_entry_t entry;
  entry.vtable = &pipe_r_vtable;
  entry.data = readhandle;
  int pipefd0 = descriptor_table_insert(entry);
  if (pipefd0<0) {
    pipe_r_free(readhandle);
    pipe_w_free(writehandle);
    return -1;
  }
  entry.vtable = &pipe_w_vtable;
  entry.data = writehandle;
  int pipefd1 = descriptor_table_insert(entry);
  if (pipefd1<0) {
    pipe_r_free(readhandle);
    pipe_w_free(writehandle);
    descriptor_table_remove(pipefd0);
    return -1;
  }
  pipefd[0] = pipefd0;
  pipefd[1] = pipefd1;
  return 0;
}

#endif
