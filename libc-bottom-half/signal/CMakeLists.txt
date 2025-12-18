add_internal_object_library(top-half-emulated-signal
    ../../libc-top-half/musl/src/signal/psignal.c
    ../../libc-top-half/musl/src/string/strsignal.c)
foreach(obj top-half-emulated-signal-shared top-half-emulated-signal-static)
  target_compile_definitions(${obj} PRIVATE _WASI_EMULATED_SIGNAL)
endforeach()

add_object_library(bottom-half-emulated-signal signal.c)

add_internal_shared_library(wasi-emulated-signal
    $<TARGET_OBJECTS:bottom-half-emulated-signal-shared>
    $<TARGET_OBJECTS:top-half-emulated-signal-shared>)

add_library(wasi-emulated-signal-static STATIC
    $<TARGET_OBJECTS:bottom-half-emulated-signal-static>
    $<TARGET_OBJECTS:top-half-emulated-signal-static>)

sysroot_lib(wasi-emulated-signal-static libwasi-emulated-signal.a)
sysroot_lib(wasi-emulated-signal libwasi-emulated-signal.so)
