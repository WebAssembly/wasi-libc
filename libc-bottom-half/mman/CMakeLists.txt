add_internal_library_pair(wasi-emulated-mman mman.c)
sysroot_lib(wasi-emulated-mman-static libwasi-emulated-mman.a)
sysroot_lib(wasi-emulated-mman libwasi-emulated-mman.so)
