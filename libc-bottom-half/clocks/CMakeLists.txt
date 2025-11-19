add_internal_library_pair(wasi-emulated-process-clocks
    clock.c
    getrusage.c
    times.c)

sysroot_lib(wasi-emulated-process-clocks-static libwasi-emulated-process-clocks.a)
sysroot_lib(wasi-emulated-process-clocks libwasi-emulated-process-clocks.so)
