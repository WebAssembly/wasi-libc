AC_INIT([WebAssembly Reference Sysroot], [0.1], [sunfish@mozilla.com])
AC_ARG_VAR([WASM_CC],[Path to clang with default wasm32 target])
AC_ARG_VAR([HOST_TRIPLE],[Use different target triple])
AC_PREFIX_DEFAULT([./sysroot])
AC_SUBST([HOST_TRIPLE],[wasm32-unknown-wasi-musl])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
