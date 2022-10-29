# These variables are specifically meant to be overridable via the make
# command-line.
# ?= doesn't work for CC and AR because make has a default value for them.
ifeq ($(origin CC), default)
CC := clang
endif
NM ?= $(patsubst %clang,%llvm-nm,$(filter-out ccache sccache,$(CC)))
ifeq ($(origin AR), default)
AR = $(patsubst %clang,%llvm-ar,$(filter-out ccache sccache,$(CC)))
endif
EXTRA_CFLAGS ?= -O2 -DNDEBUG -ftls-model=local-exec -D_WASI_EMULATED_MMAN -D_WASI_EMULATED_PROCESS_CLOCKS
# The directory where we build the sysroot.
SYSROOT ?= $(CURDIR)/sysroot
# A directory to install to for "make install".
INSTALL_DIR ?= /usr/local
# single or posix
THREAD_MODEL ?= posix
# dlmalloc or none
MALLOC_IMPL ?= dlmalloc
# yes or no
BUILD_LIBC_TOP_HALF ?= yes
# The directory where we're store intermediate artifacts.
OBJDIR ?= $(CURDIR)/build

# When the length is no larger than this threshold, we consider the
# overhead of bulk memory opcodes to outweigh the performance benefit,
# and fall back to the original musl implementation. See
# https://github.com/WebAssembly/wasi-libc/pull/263 for relevant
# discussion
BULK_MEMORY_THRESHOLD ?= 32

# Variables from this point on are not meant to be overridable via the
# make command-line.

# Set the target variables. Multiarch triples notably omit the vendor field,
# which happens to be what we do for the main target triple too.
#TARGET_ARCH = wasm32
TARGET_TRIPLE = $(TARGET_ARCH)-wasi
MULTIARCH_TRIPLE = $(TARGET_ARCH)-wasi

# These variables describe the locations of various files and directories in
# the source tree.
DLMALLOC_DIR = $(CURDIR)/dlmalloc
DLMALLOC_SRC_DIR = $(DLMALLOC_DIR)/src
DLMALLOC_SOURCES = $(DLMALLOC_SRC_DIR)/dlmalloc.c
DLMALLOC_INC = $(DLMALLOC_DIR)/include
LIBC_BOTTOM_HALF_DIR = $(CURDIR)/libc-bottom-half
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC = $(LIBC_BOTTOM_HALF_DIR)/cloudlibc/src
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC = $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC)/include
LIBC_BOTTOM_HALF_HEADERS_PUBLIC = $(LIBC_BOTTOM_HALF_DIR)/headers/public
LIBC_BOTTOM_HALF_HEADERS_PRIVATE = $(LIBC_BOTTOM_HALF_DIR)/headers/private
LIBC_BOTTOM_HALF_SOURCES = $(LIBC_BOTTOM_HALF_DIR)/sources
LIBC_BOTTOM_HALF_ALL_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) -name \*.c) \
    $(shell find $(LIBC_BOTTOM_HALF_SOURCES) -name \*.c)

# FIXME(https://reviews.llvm.org/D85567) - due to a bug in LLD the weak
# references to a function defined in `chdir.c` only work if `chdir.c` is at the
# end of the archive, but once that LLD review lands and propagates into LLVM
# then we don't have to do this.
LIBC_BOTTOM_HALF_ALL_SOURCES := $(filter-out $(LIBC_BOTTOM_HALF_SOURCES)/chdir.c,$(LIBC_BOTTOM_HALF_ALL_SOURCES))
LIBC_BOTTOM_HALF_ALL_SOURCES := $(LIBC_BOTTOM_HALF_ALL_SOURCES) $(LIBC_BOTTOM_HALF_SOURCES)/chdir.c

LIBWASI_EMULATED_MMAN_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_DIR)/mman -name \*.c)
LIBWASI_EMULATED_PROCESS_CLOCKS_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_DIR)/clocks -name \*.c)
LIBWASI_EMULATED_GETPID_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_DIR)/getpid -name \*.c)
LIBWASI_EMULATED_SIGNAL_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_DIR)/signal -name \*.c)
LIBC_BOTTOM_HALF_CRT_SOURCES = $(wildcard $(LIBC_BOTTOM_HALF_DIR)/crt/*.c)
LIBC_TOP_HALF_DIR = $(CURDIR)/libc-top-half
LIBC_TOP_HALF_MUSL_DIR = $(LIBC_TOP_HALF_DIR)/musl
LIBC_TOP_HALF_MUSL_SRC_DIR = $(LIBC_TOP_HALF_MUSL_DIR)/src
LIBC_TOP_HALF_MUSL_INC = $(LIBC_TOP_HALF_MUSL_DIR)/include
LIBC_TOP_HALF_MUSL_SOURCES = \
    $(addprefix $(LIBC_TOP_HALF_MUSL_SRC_DIR)/, \
        internal/syscall_ret.c \
        misc/a64l.c \
        misc/basename.c \
        misc/dirname.c \
        misc/ffs.c \
        misc/ffsl.c \
        misc/ffsll.c \
        misc/fmtmsg.c \
        misc/getdomainname.c \
        misc/gethostid.c \
        misc/getopt.c \
        misc/getopt_long.c \
        misc/getsubopt.c \
        misc/getrlimit.c \
        misc/setrlimit.c \
        misc/getrusage.c \
        misc/uname.c \
        misc/nftw.c \
        misc/syslog.c \
        errno/strerror.c \
        network/htonl.c \
        network/htons.c \
        network/ntohl.c \
        network/ntohs.c \
        network/inet_ntop.c \
        network/inet_pton.c \
        network/inet_aton.c \
        network/in6addr_any.c \
        network/in6addr_loopback.c \
        fenv/fenv.c \
        fenv/fesetround.c \
        fenv/feupdateenv.c \
        fenv/fesetexceptflag.c \
        fenv/fegetexceptflag.c \
        fenv/feholdexcept.c \
        exit/exit.c \
        exit/atexit.c \
        exit/assert.c \
        exit/quick_exit.c \
        exit/at_quick_exit.c \
        time/strftime.c \
        time/asctime.c \
        time/asctime_r.c \
        time/ctime.c \
        time/ctime_r.c \
        time/wcsftime.c \
        time/strptime.c \
        time/difftime.c \
        time/timegm.c \
        time/ftime.c \
        time/times.c \
        time/gmtime.c \
        time/gmtime_r.c \
        time/timespec_get.c \
        time/getdate.c \
        time/localtime.c \
        time/localtime_r.c \
        time/mktime.c \
        time/gettimeofday.c \
        time/__tm_to_secs.c \
        time/__month_to_secs.c \
        time/__secs_to_tm.c \
        time/__year_to_secs.c \
        time/__tz.c \
        fcntl/creat.c \
        dirent/alphasort.c \
        dirent/versionsort.c \
        env/clearenv.c \
        env/getenv.c \
        env/putenv.c \
        env/setenv.c \
        env/unsetenv.c \
        unistd/posix_close.c \
        unistd/tcgetpgrp.c \
        unistd/tcsetpgrp.c \
        unistd/getpgid.c \
        unistd/getpgrp.c \
        unistd/setpgid.c \
        unistd/setpgrp.c \
        unistd/getsid.c \
        unistd/setsid.c \
        unistd/gethostname.c \
        unistd/alarm.c \
        unistd/ualarm.c \
        unistd/ttyname.c \
        unistd/ttyname_r.c \
        linux/wait3.c \
        linux/wait4.c \
        linux/eventfd.c \
        stat/futimesat.c \
    ) \
    $(filter-out %/procfdname.c %/syscall.c %/syscall_ret.c %/vdso.c %/version.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal/*.c)) \
    $(filter-out %/rename.c \
                 %/tmpnam.c %/tmpfile.c %/tempnam.c \
                 %/popen.c %/pclose.c \
                 %/remove.c \
                 %/gets.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/*.c)) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/string/*.c) \
    $(filter-out %/bind_textdomain_codeset.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/locale/*.c)) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/setjmp/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/thread/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/signal/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/process/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/search/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/multibyte/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/regex/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/prng/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/conf/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/passwd/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/ctype/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/termios/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/temp/*.c) \
    $(filter-out %/__signbit.c %/__signbitf.c %/__signbitl.c \
                 %/__fpclassify.c %/__fpclassifyf.c %/__fpclassifyl.c \
                 %/ceilf.c %/ceil.c \
                 %/floorf.c %/floor.c \
                 %/truncf.c %/trunc.c \
                 %/rintf.c %/rint.c \
                 %/nearbyintf.c %/nearbyint.c \
                 %/sqrtf.c %/sqrt.c \
                 %/fabsf.c %/fabs.c \
                 %/copysignf.c %/copysign.c \
                 %/fminf.c %/fmaxf.c \
                 %/fmin.c %/fmax.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/math/*.c)) \
    $(filter-out %/crealf.c %/creal.c %creall.c \
                 %/cimagf.c %/cimag.c %cimagl.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/complex/*.c)) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/crypt/*.c)
MUSL_PRINTSCAN_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal/floatscan.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/vfprintf.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/vfwprintf.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/vfscanf.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/strtod.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/wcstod.c
BULK_MEMORY_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/string/memcpy.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/string/memmove.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/string/memset.c
LIBC_TOP_HALF_HEADERS_PRIVATE = $(LIBC_TOP_HALF_DIR)/headers/private
LIBC_TOP_HALF_SOURCES = $(LIBC_TOP_HALF_DIR)/sources
LIBC_TOP_HALF_ALL_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SOURCES) \
    $(shell find $(LIBC_TOP_HALF_SOURCES) -name \*.c)

# Add any extra flags
CFLAGS = $(EXTRA_CFLAGS)
# Set the target.
CFLAGS += --target=$(TARGET_TRIPLE)
# WebAssembly floating-point match doesn't trap.
# TODO: Add -fno-signaling-nans when the compiler supports it.
CFLAGS += -fno-trapping-math
# Add all warnings, but disable a few which occur in third-party code.
CFLAGS += -Wall -Wextra -Werror \
  -Wno-null-pointer-arithmetic \
  -Wno-unused-parameter \
  -Wno-sign-compare \
  -Wno-unused-variable \
  -Wno-unused-function \
  -Wno-ignored-attributes \
  -Wno-missing-braces \
  -Wno-ignored-pragmas \
  -Wno-unused-but-set-variable \
  -Wno-unknown-warning-option

# Configure support for threads.
ifeq ($(THREAD_MODEL), single)
CFLAGS += -mthread-model single
endif
ifeq ($(THREAD_MODEL), posix)
CFLAGS += -mthread-model posix -pthread
endif

# Expose the public headers to the implementation. We use `-isystem` for
# purpose for two reasons:
#
# 1. It only does `<...>` not `"...."` lookup. We are making a libc,
#    which is a system library, so all public headers should be
#    accessible via `<...>` and never also need `"..."`. `-isystem` main
#    purpose is to only effect `<...>` lookup.
#
# 2. The `-I` for private headers added for specific C files below
#    should come earlier in the search path, so they can "override"
#    and/or `#include_next` the public headers. `-isystem` (like
#    `-idirafter`) comes later in the search path than `-I`.
CFLAGS += -isystem "$(SYSROOT_INC)"

# These variables describe the locations of various files and directories in
# the build tree.
objs = $(patsubst $(CURDIR)/%.c,$(OBJDIR)/%.o,$(1))
DLMALLOC_OBJS = $(call objs,$(DLMALLOC_SOURCES))
LIBC_BOTTOM_HALF_ALL_OBJS = $(call objs,$(LIBC_BOTTOM_HALF_ALL_SOURCES))
LIBC_TOP_HALF_ALL_OBJS = $(call objs,$(LIBC_TOP_HALF_ALL_SOURCES))
ifeq ($(MALLOC_IMPL),dlmalloc)
LIBC_OBJS += $(DLMALLOC_OBJS)
else ifeq ($(MALLOC_IMPL),none)
# No object files to add.
else
$(error unknown malloc implementation $(MALLOC_IMPL))
endif
# Add libc-bottom-half's objects.
LIBC_OBJS += $(LIBC_BOTTOM_HALF_ALL_OBJS)
ifeq ($(BUILD_LIBC_TOP_HALF),yes)
# libc-top-half is musl.
LIBC_OBJS += $(LIBC_TOP_HALF_ALL_OBJS)
endif
MUSL_PRINTSCAN_OBJS = $(call objs,$(MUSL_PRINTSCAN_SOURCES))
MUSL_PRINTSCAN_LONG_DOUBLE_OBJS = $(patsubst %.o,%.long-double.o,$(MUSL_PRINTSCAN_OBJS))
MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS = $(patsubst %.o,%.no-floating-point.o,$(MUSL_PRINTSCAN_OBJS))
BULK_MEMORY_OBJS = $(call objs,$(BULK_MEMORY_SOURCES))
LIBWASI_EMULATED_MMAN_OBJS = $(call objs,$(LIBWASI_EMULATED_MMAN_SOURCES))
LIBWASI_EMULATED_PROCESS_CLOCKS_OBJS = $(call objs,$(LIBWASI_EMULATED_PROCESS_CLOCKS_SOURCES))
LIBWASI_EMULATED_GETPID_OBJS = $(call objs,$(LIBWASI_EMULATED_GETPID_SOURCES))
LIBWASI_EMULATED_SIGNAL_OBJS = $(call objs,$(LIBWASI_EMULATED_SIGNAL_SOURCES))

# These variables describe the locations of various files and
# directories in the generated sysroot tree.
SYSROOT_LIB := $(SYSROOT)/lib/$(MULTIARCH_TRIPLE)
SYSROOT_INC = $(SYSROOT)/include
SYSROOT_SHARE = $(SYSROOT)/share/$(MULTIARCH_TRIPLE)

# Files from musl's include directory that we don't want to install in the
# sysroot's include directory.
MUSL_OMIT_HEADERS :=

# Remove files which aren't headers (we generate alltypes.h below).
MUSL_OMIT_HEADERS += \
    "bits/syscall.h.in" \
    "bits/alltypes.h.in" \
    "alltypes.h.in"

# Use the compiler's version of these headers.
MUSL_OMIT_HEADERS += \
    "stdarg.h" \
    "stddef.h"

# Use the WASI errno definitions.
MUSL_OMIT_HEADERS += \
    "bits/errno.h"

# Remove headers that aren't supported yet or that aren't relevant for WASI.
MUSL_OMIT_HEADERS += \
    "sys/procfs.h" \
    "sys/user.h" \
    "sys/kd.h" "sys/vt.h" "sys/soundcard.h" "sys/sem.h" \
    "sys/shm.h" "sys/msg.h" "sys/ipc.h" "sys/ptrace.h" \
    "sys/statfs.h" \
    "bits/kd.h" "bits/vt.h" "bits/soundcard.h" "bits/sem.h" \
    "bits/shm.h" "bits/msg.h" "bits/ipc.h" "bits/ptrace.h" \
    "bits/statfs.h" \
    "sys/vfs.h" \
    "sys/statvfs.h" \
    "sys/syslog.h" \
    "ucontext.h" "sys/ucontext.h" \
    "utmp.h" "utmpx.h" \
    "lastlog.h" \
    "sys/acct.h" \
    "sys/cachectl.h" \
    "sys/epoll.h" "sys/reboot.h" "sys/swap.h" \
    "sys/sendfile.h" "sys/inotify.h" \
    "sys/quota.h" \
    "sys/klog.h" \
    "sys/fsuid.h" \
    "sys/io.h" \
    "sys/prctl.h" \
    "sys/mtio.h" \
    "sys/mount.h" \
    "sys/fanotify.h" \
    "sys/personality.h" \
    "link.h" "bits/link.h" \
    "scsi/scsi.h" "scsi/scsi_ioctl.h" "scsi/sg.h" \
    "sys/auxv.h" \
    "mntent.h" \
    "resolv.h" \
    "pty.h" \
    "dlfcn.h" \
    "ulimit.h" \
    "sys/xattr.h" \
    "wordexp.h" \
    "sys/membarrier.h" \
    "sys/signalfd.h" \
    "sys/termios.h" \
    "net/if.h" \
    "net/if_arp.h" \
    "net/ethernet.h" \
    "net/route.h" \
    "netinet/if_ether.h" \
    "netinet/ether.h" \
    "sys/timerfd.h" \
    "sys/sysmacros.h"

ifeq ($(THREAD_MODEL), single)
# Remove headers not supported in single-threaded mode.
MUSL_OMIT_HEADERS += "aio.h" "pthread.h"
endif

default: finish

$(SYSROOT_LIB)/libc.a: $(LIBC_OBJS)

$(SYSROOT_LIB)/libc-printscan-long-double.a: $(MUSL_PRINTSCAN_LONG_DOUBLE_OBJS)

$(SYSROOT_LIB)/libc-printscan-no-floating-point.a: $(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS)

$(SYSROOT_LIB)/libwasi-emulated-mman.a: $(LIBWASI_EMULATED_MMAN_OBJS)

$(SYSROOT_LIB)/libwasi-emulated-process-clocks.a: $(LIBWASI_EMULATED_PROCESS_CLOCKS_OBJS)

$(SYSROOT_LIB)/libwasi-emulated-getpid.a: $(LIBWASI_EMULATED_GETPID_OBJS)

%.a:
	@mkdir -p "$(@D)"
	# On Windows, the commandline for the ar invocation got too long, so it needs to be split up.
	$(AR) crs $@ $(wordlist 1, 199, $^)
	$(AR) crs $@ $(wordlist 200, 399, $^)
	$(AR) crs $@ $(wordlist 400, 599, $^)
	$(AR) crs $@ $(wordlist 600, 799, $^)
	# This might eventually overflow again, but at least it'll do so in a loud way instead of
	# silently dropping the tail.
	$(AR) crs $@ $(wordlist 800, 100000, $^)

$(MUSL_PRINTSCAN_OBJS): CFLAGS += \
	    -D__wasilibc_printscan_full_support_option="\"add -lc-printscan-long-double to the link command\""

$(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS): CFLAGS += \
	    -D__wasilibc_printscan_floating_point_support_option="\"remove -lc-printscan-no-floating-point from the link command\""

# TODO: apply -mbulk-memory globally, once
# https://github.com/llvm/llvm-project/issues/52618 is resolved
$(BULK_MEMORY_OBJS): CFLAGS += \
        -mbulk-memory

$(BULK_MEMORY_OBJS): CFLAGS += \
        -DBULK_MEMORY_THRESHOLD=$(BULK_MEMORY_THRESHOLD)

$(OBJDIR)/%.long-double.o: $(CURDIR)/%.c include_dirs
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -MD -MP -o $@ -c $<

$(OBJDIR)/%.no-floating-point.o: $(CURDIR)/%.c include_dirs
	@mkdir -p "$(@D)"	
	$(CC) $(CFLAGS) -MD -MP -o $@ -c $<

$(OBJDIR)/%.o: $(CURDIR)/%.c include_dirs
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -MD -MP -o $@ -c $<

-include $(shell find $(OBJDIR) -name \*.d)

$(DLMALLOC_OBJS): CFLAGS += \
    -I$(DLMALLOC_INC)

startup_files $(LIBC_BOTTOM_HALF_ALL_OBJS): CFLAGS += \
    -I$(LIBC_BOTTOM_HALF_HEADERS_PRIVATE) \
    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC) \
    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/include \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal

$(LIBC_TOP_HALF_ALL_OBJS) $(MUSL_PRINTSCAN_LONG_DOUBLE_OBJS) $(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS): CFLAGS += \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/include \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal \
    -I$(LIBC_TOP_HALF_MUSL_DIR)/arch/$(TARGET_ARCH) \
    -I$(LIBC_TOP_HALF_MUSL_DIR)/arch/generic \
    -I$(LIBC_TOP_HALF_HEADERS_PRIVATE) \
    -Wno-parentheses \
    -Wno-shift-op-parentheses \
    -Wno-bitwise-op-parentheses \
    -Wno-logical-op-parentheses \
    -Wno-string-plus-int \
    -Wno-dangling-else \
    -Wno-unknown-pragmas

$(LIBWASI_EMULATED_PROCESS_CLOCKS_OBJS): CFLAGS += \
    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC)

include_dirs:
	#
	# Install the include files.
	#
	mkdir -p "$(SYSROOT_INC)"
	cp -r "$(LIBC_BOTTOM_HALF_HEADERS_PUBLIC)"/* "$(SYSROOT_INC)"

	# Generate musl's bits/alltypes.h header.
	mkdir -p "$(SYSROOT_INC)/bits"
	sed -f $(LIBC_TOP_HALF_MUSL_DIR)/tools/mkalltypes.sed \
	    $(LIBC_TOP_HALF_MUSL_DIR)/arch/$(TARGET_ARCH)/bits/alltypes.h.in \
	    $(LIBC_TOP_HALF_MUSL_DIR)/include/alltypes.h.in \
	    > "$(SYSROOT_INC)/bits/alltypes.h"

	# Copy in the bulk of musl's public header files.
	cp -r "$(LIBC_TOP_HALF_MUSL_INC)"/* "$(SYSROOT_INC)"
	# Copy in the musl's "bits" header files.
	cp -r "$(LIBC_TOP_HALF_MUSL_DIR)"/arch/generic/bits/* "$(SYSROOT_INC)/bits"
	cp -r "$(LIBC_TOP_HALF_MUSL_DIR)"/arch/$(TARGET_ARCH)/bits/* "$(SYSROOT_INC)/bits"

	# Remove selected header files.
	$(RM) $(patsubst %,$(SYSROOT_INC)/%,$(MUSL_OMIT_HEADERS))

startup_files: include_dirs
	#
	# Build the startup files.
	#
	@mkdir -p "$(OBJDIR)"
	cd "$(OBJDIR)" && \
	$(CC) $(CFLAGS) -c $(LIBC_BOTTOM_HALF_CRT_SOURCES) -MD -MP && \
	mkdir -p "$(SYSROOT_LIB)" && \
	mv *.o "$(SYSROOT_LIB)"

libc: include_dirs \
    $(SYSROOT_LIB)/libc.a \
    $(SYSROOT_LIB)/libc-printscan-long-double.a \
    $(SYSROOT_LIB)/libc-printscan-no-floating-point.a \
    $(SYSROOT_LIB)/libwasi-emulated-mman.a \
    $(SYSROOT_LIB)/libwasi-emulated-process-clocks.a \
    $(SYSROOT_LIB)/libwasi-emulated-getpid.a

finish: startup_files libc
	#
	# Create empty placeholder libraries.
	#
	for name in m rt pthread crypt util xnet resolv dl; do \
	    $(AR) crs "$(SYSROOT_LIB)/lib$${name}.a"; \
	done

	#
	# The build succeeded! The generated sysroot is in $(SYSROOT).
	#

# The check for defined and undefined symbols expects there to be a heap
# alloctor (providing malloc, calloc, free, etc). Skip this step if the build
# is done without a malloc implementation.
ifneq ($(MALLOC_IMPL),none)
finish: check-symbols
endif

DEFINED_SYMBOLS = $(SYSROOT_SHARE)/defined-symbols.txt
UNDEFINED_SYMBOLS = $(SYSROOT_SHARE)/undefined-symbols.txt

check-symbols: startup_files libc
	#
	# Collect metadata on the sysroot and perform sanity checks.
	#
	mkdir -p "$(SYSROOT_SHARE)"

	#
	# Collect symbol information.
	#
	@# TODO: Use llvm-nm --extern-only instead of grep. This is blocked on
	@# LLVM PR40497, which is fixed in 9.0, but not in 8.0.
	@# Ignore certain llvm builtin symbols such as those starting with __mul
	@# since these dependencies can vary between llvm versions.
	"$(NM)" --defined-only "$(SYSROOT_LIB)"/libc.a "$(SYSROOT_LIB)"/libwasi-emulated-*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' [[:upper:]] ' |sed 's/.* [[:upper:]] //' |LC_ALL=C sort > "$(DEFINED_SYMBOLS)"
	for undef_sym in $$("$(NM)" --undefined-only "$(SYSROOT_LIB)"/libc.a "$(SYSROOT_LIB)"/libc-*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' U ' |sed 's/.* U //' |LC_ALL=C sort |uniq); do \
	    grep -q '\<'$$undef_sym'\>' "$(DEFINED_SYMBOLS)" || echo $$undef_sym; \
	done | grep -v "^__mul" > "$(UNDEFINED_SYMBOLS)"
	grep '^_*imported_$(TARGET_OS)_' "$(UNDEFINED_SYMBOLS)" \
	    > "$(SYSROOT_LIB)/libc.imports"

	#
	# Generate a test file that includes all public C header files.
	#
	cd "$(SYSROOT_INC)" && \
	  for header in $$(find . -type f -not -name mman.h -not -name signal.h -not -name times.h -not -name resource.h |grep -v /bits/ |grep -v /c++/); do \
	      echo '#include <'$$header'>' | sed 's/\.\///' ; \
	done |LC_ALL=C sort >$(SYSROOT_SHARE)/include-all.c ; \
	cd - >/dev/null

	#
	# Test that it compiles.
	#
	$(CC) $(CFLAGS) -fsyntax-only "$(SYSROOT_SHARE)/include-all.c" -Wno-\#warnings

	#
	# Collect all the predefined macros, except for compiler version macros
	# which we don't need to track here.
	#
	@#
	@# For the __*_ATOMIC_*_LOCK_FREE macros, squash individual compiler names
	@# to attempt, toward keeping these files compiler-independent.
	@#
	@# We have to add `-isystem $(SYSROOT_INC)` because otherwise clang puts
	@# its builtin include path first, which produces compiler-specific
	@# output.
	@#
	@# TODO: Undefine __FLOAT128__ for now since it's not in clang 8.0.
	@# TODO: Filter out __FLT16_* for now, as not all versions of clang have these.
	@# TODO: Filter out __NO_MATH_ERRNO_ and a few __*WIDTH__ that are new to clang 14.
	@# TODO: clang defined __FLT_EVAL_METHOD__ until clang 15, so we force-undefine it
	@# for older versions.
	$(CC) $(CFLAGS) "$(SYSROOT_SHARE)/include-all.c" \
	    -isystem $(SYSROOT_INC) \
	    -std=gnu17 \
	    -E -dM -Wno-\#warnings \
	    -D_ALL_SOURCE \
	    -U__llvm__ \
	    -U__clang__ \
	    -U__clang_major__ \
	    -U__clang_minor__ \
	    -U__clang_patchlevel__ \
	    -U__clang_version__ \
	    -U__clang_literal_encoding__ \
	    -U__clang_wide_literal_encoding__ \
	    -U__GNUC__ \
	    -U__GNUC_MINOR__ \
	    -U__GNUC_PATCHLEVEL__ \
	    -U__VERSION__ \
	    -U__FLOAT128__ \
	    -U__NO_MATH_ERRNO__ \
	    -U__BITINT_MAXWIDTH__ \
	    -U__FLT_EVAL_METHOD__ -Wno-builtin-macro-redefined \
	    | sed -e 's/__[[:upper:][:digit:]]*_ATOMIC_\([[:upper:][:digit:]_]*\)_LOCK_FREE/__compiler_ATOMIC_\1_LOCK_FREE/' \
	    | grep -v '^#define __FLT16_' \
	    | grep -v '^#define __\(BOOL\|INT_\(LEAST\|FAST\)\(8\|16\|32\|64\)\|INT\|LONG\|LLONG\|SHRT\)_WIDTH__' \
	    > "$(SYSROOT_SHARE)/predefined-macros.txt"

	# Check that the computed metadata matches the expected metadata.
	# This ignores whitespace because on Windows the output has CRLF line endings.
	diff -wur "$(CURDIR)/expected/$(MULTIARCH_TRIPLE)" "$(SYSROOT_SHARE)"

install: finish
	mkdir -p "$(INSTALL_DIR)"
	cp -r "$(SYSROOT)/lib" "$(SYSROOT)/share" "$(SYSROOT)/include" "$(INSTALL_DIR)"

clean:
	$(RM) -r "$(OBJDIR)"
	$(RM) -r "$(SYSROOT)"
	$(RM) -r "$(SYSROOT)32"
	$(RM) -r "$(SYSROOT)64"

.PHONY: default startup_files libc finish install include_dirs clean
