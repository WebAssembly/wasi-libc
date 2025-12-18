add_internal_library_pair(wasi-emulated-getpid getpid.c)
sysroot_lib(wasi-emulated-getpid-static libwasi-emulated-getpid.a)
sysroot_lib(wasi-emulated-getpid libwasi-emulated-getpid.so)
