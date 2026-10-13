#include <stdarg.h>
#include <stdio.h>
#include <wasi/report-error.h>
#include <wasi/report-errorf.h>
#include <wasi/utf8.h>

void __wasilibc_report_errorf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);

#if defined(__webp2__)
#define ERROR_MAX_LEN 512
  char format_buf[ERROR_MAX_LEN];
  uint8_t utf8_buf[ERROR_MAX_LEN + 1]; // leave room for nul

  int n = vsnprintf(format_buf, sizeof(format_buf), format, ap);
  wasilibc_utf8_writer_t utf8 =
      __wasilibc_make_utf8_writer(utf8_buf, ERROR_MAX_LEN);

  // Re-encode as UTF-8 (excluding vnsprintf's nul)
  for (int i = 0; i < n && i < (int)sizeof(format_buf) - 1; i++) {
    if (!__wasilibc_utf8_write(&utf8, format_buf[i])) {
      break;
    }
  }
  __wasilibc_utf8_abort_current(&utf8);

  // Nul-terminate message and pass along
  utf8_buf[utf8.valid_len] = 0;
  __wasilibc_report_error((char *)utf8_buf);
#else
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
#endif

  va_end(ap);
}
