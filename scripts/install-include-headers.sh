#!/usr/bin/env bash
#
# Install all the necessary necessary include files from wasi-libc
# (`$WASI_LIBC`) into the sysroot include directory (`$SYSROOT_INC`). It uses a
# passed target triple (`$TARGET_TRIPLE`) to condition some of the
# copied/generated files.
#
# Usage: SYSROOT_INC=... TARGET_TRIPLE=... ./install-include-headers.sh

set -euo pipefail

PARENT_DIR=$(dirname $(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd))
WASI_LIBC="${WASI_LIBC:-$PARENT_DIR}"
if [[ -z "${SYSROOT_INC}" || -z "${TARGET_TRIPLE}" ]]; then
    echo "usage: SYSROOT_INC=... TARGET_TRIPLE=... ./install-include-headers.sh"
    exit 1
fi
# The commands are available for override to allow dry runs.
CP="${CP:-cp -v}"
RM="${RM:-rm -v}"
MKDIR="${MKDIR:-mkdir}"
SED="${SED:-sed}"

# Copy in the bottom half's public headers.
$MKDIR -p $SYSROOT_INC
$CP -r $WASI_LIBC/libc-bottom-half/headers/public/* $SYSROOT_INC

# Copy in the bulk of musl's public header files.
$CP -r $WASI_LIBC/libc-top-half/musl/include/* $SYSROOT_INC

# Copy in the musl's "bits" header files.
$MKDIR -p $SYSROOT_INC/bits
$CP -r $WASI_LIBC/libc-top-half/musl/arch/generic/bits/* $SYSROOT_INC/bits
$CP -r $WASI_LIBC/libc-top-half/musl/arch/wasm32/bits/* $SYSROOT_INC/bits

# Generate musl's bits/alltypes.h header.
(set -x; $SED -f $WASI_LIBC/libc-top-half/musl/tools/mkalltypes.sed \
    $WASI_LIBC/libc-top-half/musl/arch/wasm32/bits/alltypes.h.in \
    $WASI_LIBC/libc-top-half/musl/include/alltypes.h.in \
    > $SYSROOT_INC/bits/alltypes.h)

# Copy in the fts header files.
$CP $WASI_LIBC/fts/musl-fts/fts.h $SYSROOT_INC/fts.h

# Remove selected header files. These are files from musl's include directory
# that we don't want to install in the sysroot's include directory.
MUSL_OMIT_HEADERS=()
# Remove files which aren't headers (we generate `alltypes.h` above).
MUSL_OMIT_HEADERS+=("bits/syscall.h.in" "bits/alltypes.h.in" "alltypes.h.in")
# Use the compiler's version of these headers.
MUSL_OMIT_HEADERS+=("stdarg.h" "stddef.h")
# Use the WASI errno definitions.
MUSL_OMIT_HEADERS+=("bits/errno.h")
# Remove headers that aren't supported yet or that aren't relevant for WASI.
MUSL_OMIT_HEADERS+=("sys/procfs.h" "sys/user.h" "sys/kd.h" "sys/vt.h" \
    "sys/soundcard.h" "sys/sem.h" "sys/shm.h" "sys/msg.h" "sys/ipc.h" \
    "sys/ptrace.h" "sys/statfs.h" "bits/kd.h" "bits/vt.h" "bits/soundcard.h" \
    "bits/sem.h" "bits/shm.h" "bits/msg.h" "bits/ipc.h" "bits/ptrace.h" \
    "bits/statfs.h" "sys/vfs.h" "syslog.h" "sys/syslog.h" "wait.h" \
    "sys/wait.h" "ucontext.h" "sys/ucontext.h" "paths.h" "utmp.h" "utmpx.h" \
    "lastlog.h" "sys/acct.h" "sys/cachectl.h" "sys/epoll.h" "sys/reboot.h" \
    "sys/swap.h" "sys/sendfile.h" "sys/inotify.h" "sys/quota.h" "sys/klog.h" \
    "sys/fsuid.h" "sys/io.h" "sys/prctl.h" "sys/mtio.h" "sys/mount.h" \
    "sys/fanotify.h" "sys/personality.h" "elf.h" "link.h" "bits/link.h" \
    "scsi/scsi.h" "scsi/scsi_ioctl.h" "scsi/sg.h" "sys/auxv.h" "pwd.h" \
    "shadow.h" "grp.h" "mntent.h" "resolv.h" "pty.h" "ulimit.h" "sys/xattr.h" \
    "wordexp.h" "spawn.h" "sys/membarrier.h" "sys/signalfd.h" "termios.h" \
    "sys/termios.h" "bits/termios.h" "net/if.h" "net/if_arp.h" \
    "net/ethernet.h" "net/route.h" "netinet/if_ether.h" "netinet/ether.h" \
    "sys/timerfd.h" "libintl.h" "sys/sysmacros.h" "aio.h")
# Exclude `netdb.h` from all of the p1 targets.
if [[ $TARGET_TRIPLE == *"wasi" || $TARGET_TRIPLE == *"wasi-threads" || \
      $TARGET_TRIPLE == *"wasip1" || $TARGET_TRIPLE == *"wasip1-threads" ]]; then
    MUSL_OMIT_HEADERS+=("netdb.h")
fi

# Remove all the `MUSL_OMIT_HEADERS` previously copied over.
for OMIT_HEADER in "${MUSL_OMIT_HEADERS[@]}"; do
    $RM -f $SYSROOT_INC/$OMIT_HEADER
done

# Update the `__wasi_snapshot.h` with some additional p2 definitions.
if [[ $TARGET_TRIPLE == *"p2" ]]; then
    printf '#ifndef __wasilibc_use_wasip2\n#define __wasilibc_use_wasip2\n#endif\n' \
        > $SYSROOT_INC/__wasi_snapshot.h
fi
