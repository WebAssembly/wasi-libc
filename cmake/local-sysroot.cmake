# "local" sysroot for files required for building libc
# (before install step)
set(SYSROOT_DIR ${CMAKE_BINARY_DIR}/sysroot)
set(SYSROOT_INC_DIR ${SYSROOT_DIR}/include)
set(SYSROOT_SHARE_DIR ${SYSROOT_DIR}/${INSTALL_SHARE_DIR})
set(SYSROOT_LIB_DIR ${SYSROOT_DIR}/${INSTALL_LIB_DIR})


# Files from musl's include directory that we don't want to install in the
# sysroot's include directory.
set(MUSL_OMIT_HEADERS
  # Remove files which aren't headers (we generate alltypes.h below).
  bits/syscall.h.in
  bits/alltypes.h.in
  alltypes.h.in

  # Use the compiler's version of these headers.
  stdarg.h
  stddef.h

  # Use the WASI errno definitions.
  bits/errno.h

  # Remove headers that aren't supported yet or that aren't relevant for WASI.
  sys/procfs.h
  sys/user.h
  sys/kd.h sys/vt.h sys/soundcard.h sys/sem.h
  sys/shm.h sys/msg.h sys/ipc.h sys/ptrace.h
  sys/statfs.h
  bits/kd.h bits/vt.h bits/soundcard.h bits/sem.h
  bits/shm.h bits/msg.h bits/ipc.h bits/ptrace.h
  bits/statfs.h
  sys/vfs.h
  sys/statvfs.h
  syslog.h sys/syslog.h
  wait.h sys/wait.h
  ucontext.h sys/ucontext.h
  paths.h
  utmp.h utmpx.h
  lastlog.h
  sys/acct.h
  sys/cachectl.h
  sys/epoll.h sys/reboot.h sys/swap.h
  sys/sendfile.h sys/inotify.h
  sys/quota.h
  sys/klog.h
  sys/fsuid.h
  sys/io.h
  sys/prctl.h
  sys/mtio.h
  sys/mount.h
  sys/fanotify.h
  sys/personality.h
  elf.h link.h bits/link.h
  scsi/scsi.h scsi/scsi_ioctl.h scsi/sg.h
  sys/auxv.h
  pwd.h shadow.h grp.h
  mntent.h
  netdb.h
  resolv.h
  pty.h
  dlfcn.h
  setjmp.h
  ulimit.h
  sys/xattr.h
  wordexp.h
  spawn.h
  sys/membarrier.h
  sys/signalfd.h
  termios.h
  sys/termios.h
  bits/termios.h
  net/if.h
  net/if_arp.h
  net/ethernet.h
  net/route.h
  netinet/if_ether.h
  netinet/ether.h
  sys/timerfd.h
  libintl.h
  sys/sysmacros.h
  aio.h)

if("${THREAD_MODEL}" STREQUAL "single")
  # Remove headers not supported in single-threaded mode.
  list(APPEND MUSL_OMIT_HEADERS "pthread.h")
endif()

set(MUSL_REMOVE_HEADERS)
foreach(HEADER ${MUSL_OMIT_HEADERS})
  list(APPEND MUSL_REMOVE_HEADERS ${SYSROOT_INC_DIR}/${HEADER})
endforeach()

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/alltypes.h.gen
  COMMAND
    sed -f ${LIBC_TOP_HALF_MUSL_DIR}/tools/mkalltypes.sed ${LIBC_TOP_HALF_MUSL_DIR}/arch/wasm32/bits/alltypes.h.in ${LIBC_TOP_HALF_MUSL_DIR}/include/alltypes.h.in > ${CMAKE_BINARY_DIR}/alltypes.h.gen
    DEPENDS
      ${LIBC_TOP_HALF_MUSL_DIR}/tools/mkalltypes.sed
      ${LIBC_TOP_HALF_MUSL_DIR}/arch/wasm32/bits/alltypes.h.in
      ${LIBC_TOP_HALF_MUSL_DIR}/include/alltypes.h.in
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/sysroot

  COMMAND
    ${CMAKE_COMMAND} -E make_directory
      ${SYSROOT_INC_DIR}
      ${SYSROOT_DIR}/share

  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      ${LIBC_BOTTOM_HALF_HEADERS_PUBLIC_DIR}
      ${SYSROOT_INC_DIR}

  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      ${LIBC_TOP_HALF_MUSL_DIR}/include/
      ${SYSROOT_INC_DIR}

  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      ${LIBC_TOP_HALF_MUSL_DIR}/arch/generic/bits/
      ${SYSROOT_INC_DIR}/bits

  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      ${LIBC_TOP_HALF_MUSL_DIR}/arch/wasm32/bits/
      ${SYSROOT_INC_DIR}/bits

  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_BINARY_DIR}/alltypes.h.gen
      ${SYSROOT_INC_DIR}/bits/alltypes.h

  COMMAND
    ${CMAKE_COMMAND} -E remove
      ${MUSL_REMOVE_HEADERS}

  DEPENDS
    ${CMAKE_BINARY_DIR}/alltypes.h.gen
)

add_custom_target(sysroot-headers ALL
  DEPENDS ${CMAKE_BINARY_DIR}/sysroot)