file(GLOB_RECURSE
    SIGNAL_SOURCES
    ${LIBC_BOTTOM_HALF_DIR}/signal/*.c)

add_library(wasi-emulated-signal-objs
  OBJECT
    ${SIGNAL_SOURCES}
)

add_dependencies(wasi-emulated-signal-objs sysroot-headers)
