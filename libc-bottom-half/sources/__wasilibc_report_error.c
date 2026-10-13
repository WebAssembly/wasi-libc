#include <stdio.h>
#include <wasi/api.h>
#include <wasi/report-error.h>

void __wasilibc_report_error(const char *message) {
#if defined(__wasip1__) || defined(__wasip2__) || defined(__wasip3__)
  fprintf(stderr, "%s\n", message);
#elif defined(__webp2__)
  __wasilibc_webp2_report_error(message);
#endif
}

#ifdef __webp2__
void __wasilibc_webp2_report_error(const char *message) {
  __wasilibc_webp2_finish_and_flush_stdio(/*also_musl=*/true);
  webp2_string_t msg;
  webp2_string_set(&msg, message);
  webp2_own_error_t err = webp2_constructor_error(&msg, NULL);
  webp2_report_error(webp2_borrow_error(err));
  webp2_error_drop_own(err);
}
#endif
