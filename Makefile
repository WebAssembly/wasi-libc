# These variables are specifically meant to be overridable via
# the make command-line.
WASM_CC = clang
WASM_NM = $(patsubst %clang,%llvm-nm,$(WASM_CC))
WASM_AR = $(patsubst %clang,%llvm-ar,$(WASM_CC))
WASM_CFLAGS = -O2
# The directory where we build the sysroot.
SYSROOT = $(CURDIR)/sysroot
# A directory to install to for "make install".
INSTALL_DIR = /usr/local
# single or posix
THREAD_MODEL = single
# yes or no
BUILD_DLMALLOC = yes
BUILD_LIBC_BOTTOM_HALF = yes
BUILD_LIBC_TOP_HALF = yes
# The directory where we're store intermediate artifacts.
OBJDIR = $(CURDIR)/build

# Check dependencies.
ifeq ($(BUILD_LIBC_TOP_HALF),yes)
ifneq ($(BUILD_LIBC_BOTTOM_HALF),yes)
$(error BUILD_LIBC_TOP_HALF=yes depends on BUILD_LIBC_BOTTOM_HALF=yes)
endif
endif
ifeq ($(BUILD_LIBC_BOTTOM_HALF),yes)
ifneq ($(BUILD_DLMALLOC),yes)
$(error BUILD_LIBC_BOTTOM_HALF=yes depends on BUILD_DLMALLOC=yes)
endif
endif

# These variables describe the locations of various files and
# directories in the source tree.
BASICS_DIR = $(CURDIR)/basics
BASICS_INC = $(BASICS_DIR)/include
BASICS_LIBC_DIR = $(BASICS_DIR)/libc
BASICS_CRT_SOURCES = $(wildcard $(BASICS_LIBC_DIR)/crt*.c)
BASICS_LIBC_SOURCES = $(BASICS_LIBC_DIR)/string.c
DLMALLOC_DIR = $(CURDIR)/dlmalloc
DLMALLOC_SRC_DIR = $(DLMALLOC_DIR)/src
DLMALLOC_SOURCES = $(DLMALLOC_SRC_DIR)/dlmalloc.c
DLMALLOC_INC = $(DLMALLOC_DIR)/include
LIBC_BOTTOM_HALF_DIR = $(CURDIR)/libc-bottom-half
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC = $(LIBC_BOTTOM_HALF_DIR)/cloudlibc/src
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC = $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC)/include
LIBC_BOTTOM_HALF_HEADERS_PUBLIC = $(LIBC_BOTTOM_HALF_DIR)/headers/public
LIBC_BOTTOM_HALF_HEADERS_PRIVATE = $(LIBC_BOTTOM_HALF_DIR)/headers/private
LIBC_BOTTOM_HALF_LIBPREOPEN_DIR = $(LIBC_BOTTOM_HALF_DIR)/libpreopen
LIBC_BOTTOM_HALF_LIBPREOPEN_LIB = $(LIBC_BOTTOM_HALF_LIBPREOPEN_DIR)/lib
LIBC_BOTTOM_HALF_LIBPREOPEN_INC = $(LIBC_BOTTOM_HALF_LIBPREOPEN_DIR)/include
LIBC_BOTTOM_HALF_SOURCES = $(LIBC_BOTTOM_HALF_DIR)/sources
LIBC_BOTTOM_HALF_ALL_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) -name \*.c) \
    $(LIBC_BOTTOM_HALF_LIBPREOPEN_LIB)/po_libc_wrappers.c \
    $(shell find $(LIBC_BOTTOM_HALF_SOURCES) -name \*.c)
LIBWASI_EMULATED_MMAN_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_DIR)/mman -name \*.c)
LIBC_BOTTOM_HALF_CRT_SOURCES = $(wildcard $(LIBC_BOTTOM_HALF_DIR)/crt/*.c)
LIBC_TOP_HALF_DIR = $(CURDIR)/libc-top-half
LIBC_TOP_HALF_MUSL_DIR = $(LIBC_TOP_HALF_DIR)/musl
LIBC_TOP_HALF_MUSL_SRC_DIR = $(LIBC_TOP_HALF_MUSL_DIR)/src
LIBC_TOP_HALF_MUSL_INC = $(LIBC_TOP_HALF_MUSL_DIR)/include
LIBC_TOP_HALF_MUSL_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/a64l.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/basename.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/dirname.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/ffs.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/ffsl.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/ffsll.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/fmtmsg.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/getdomainname.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/gethostid.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/getopt.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/getopt_long.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/getsubopt.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/uname.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/misc/nftw.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/errno/strerror.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/network/htonl.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/network/htons.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/network/ntohl.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/network/ntohs.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/fenv/fenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/fenv/fesetround.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/exit.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/atexit.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/assert.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/quick_exit.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/exit/at_quick_exit.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/strftime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/asctime_r.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/ctime_r.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/wcsftime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/strptime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/difftime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/timegm.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/ftime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/gmtime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/gmtime_r.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/timespec_get.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/getdate.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/localtime_r.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/mktime.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/__tm_to_secs.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/__month_to_secs.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/__secs_to_tm.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/__year_to_secs.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/time/__tz.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/fcntl/creat.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/dirent/alphasort.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/dirent/versionsort.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/__environ.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/clearenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/getenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/putenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/setenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/env/unsetenv.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/unistd/posix_close.c \
    $(filter-out %/procfdname.c %/syscall.c %/syscall_ret.c %/vdso.c %/version.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal/*.c)) \
    $(filter-out %/flockfile.c %/funlockfile.c %/__lockfile.c %/ftrylockfile.c \
                 %/rename.c \
                 %/tmpnam.c %/tmpfile.c %/tempnam.c \
                 %/popen.c %/pclose.c \
                 %/remove.c \
                 %/gets.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/*.c)) \
    $(filter-out %/strsignal.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/string/*.c)) \
    $(filter-out %/dcngettext.c %/textdomain.c, \
                 $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/locale/*.c)) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/search/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/multibyte/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/regex/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/prng/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/conf/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/ctype/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/math/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/complex/*.c) \
    $(wildcard $(LIBC_TOP_HALF_MUSL_SRC_DIR)/crypt/*.c)
MUSL_PRINTSCAN_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal/floatscan.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/vfprintf.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdio/vfscanf.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/strtod.c \
    $(LIBC_TOP_HALF_MUSL_SRC_DIR)/stdlib/wcstod.c
LIBC_TOP_HALF_HEADERS_PRIVATE = $(LIBC_TOP_HALF_DIR)/headers/private
LIBC_TOP_HALF_SOURCES = $(LIBC_TOP_HALF_DIR)/sources
LIBC_TOP_HALF_ALL_SOURCES = \
    $(LIBC_TOP_HALF_MUSL_SOURCES) \
    $(shell find $(LIBC_TOP_HALF_SOURCES) -name \*.c)

# Set the target variables. The multiarch triple is the same as the
# regular triple for wasm, except that it excludes the vendor field.
TARGET_TRIPLE = wasm32-unknown-wasi
MULTIARCH_TRIPLE = wasm32-wasi

# These variables describe the locations of various files and
# directories in the generated sysroot tree.
SYSROOT_LIB = $(SYSROOT)/lib/$(MULTIARCH_TRIPLE)
SYSROOT_INC = $(SYSROOT)/include
SYSROOT_SHARE = $(SYSROOT)/share/$(MULTIARCH_TRIPLE)

# Set the target.
override WASM_CFLAGS += --target=$(TARGET_TRIPLE)
# We're compiling libc.
override WASM_CFLAGS += -fno-builtin
# WebAssembly floating-point match doesn't trap.
# TODO: Add -fno-signaling-nans when the compiler supports it.
override WASM_CFLAGS += -fno-trapping-math

# Configure support for threads.
ifeq ($(THREAD_MODEL), single)
override WASM_CFLAGS += -mthread-model single
endif
ifeq ($(THREAD_MODEL), posix)
override WASM_CFLAGS += -mthread-model posix -pthread
endif

# Set the sysroot.
override WASM_CFLAGS += --sysroot="$(SYSROOT)"

objs = $(patsubst $(CURDIR)/%.c,$(OBJDIR)/%.o,$(1))
override BASICS_LIBC_OBJS = $(call objs,$(BASICS_LIBC_SOURCES))
override DLMALLOC_OBJS = $(call objs,$(DLMALLOC_SOURCES))
override LIBC_BOTTOM_HALF_ALL_OBJS = $(call objs,$(LIBC_BOTTOM_HALF_ALL_SOURCES))
override LIBC_TOP_HALF_ALL_OBJS = $(call objs,$(LIBC_TOP_HALF_ALL_SOURCES))
override LIBC_OBJS := $(BASICS_LIBC_OBJS)
ifeq ($(BUILD_DLMALLOC),yes)
override LIBC_OBJS += $(DLMALLOC_OBJS)
endif
ifeq ($(BUILD_LIBC_BOTTOM_HALF),yes)
# Override basics' string.o with libc-bottom-half's.
override LIBC_OBJS := $(filter-out %/string.o,$(LIBC_OBJS))
# Add libc-bottom-half's objects.
override LIBC_OBJS += $(LIBC_BOTTOM_HALF_ALL_OBJS)
endif
ifeq ($(BUILD_LIBC_TOP_HALF),yes)
# Override libc-bottom-half's string.o with libc-top-half's.
override LIBC_OBJS := $(filter-out %/string.o,$(LIBC_OBJS))
# Override libc-bottom-half's qsort.o with libc-top-half's.
override LIBC_OBJS := $(filter-out %/qsort.o,$(LIBC_OBJS))
# libc-top-half is musl.
override LIBC_OBJS += $(LIBC_TOP_HALF_ALL_OBJS)
endif
override MUSL_PRINTSCAN_OBJS = $(call objs,$(MUSL_PRINTSCAN_SOURCES))
override MUSL_PRINTSCAN_LONG_DOUBLE_OBJS = $(patsubst %.o,%.long-double.o,$(MUSL_PRINTSCAN_OBJS))
override MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS = $(patsubst %.o,%.no-floating-point.o,$(MUSL_PRINTSCAN_OBJS))
override LIBWASI_EMULATED_MMAN_OBJS = $(call objs,$(LIBWASI_EMULATED_MMAN_SOURCES))

default: check

$(SYSROOT_LIB)/libc.a: $(LIBC_OBJS)

$(SYSROOT_LIB)/libc-printscan-long-double.a: $(MUSL_PRINTSCAN_LONG_DOUBLE_OBJS)

$(SYSROOT_LIB)/libc-printscan-no-floating-point.a: $(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS)

$(SYSROOT_LIB)/libwasi-emulated-mman.a: $(LIBWASI_EMULATED_MMAN_OBJS)

%.a:
	@mkdir -p "$(@D)"
	$(WASM_AR) crs $@ $^

$(MUSL_PRINTSCAN_OBJS): override WASM_CFLAGS += \
	    -D__wasilibc_printscan_no_long_double \
	    -D__wasilibc_printscan_full_support_option="\"add -lc-printscan-long-double to the link command\""

$(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS): override WASM_CFLAGS += \
	    -D__wasilibc_printscan_no_floating_point \
	    -D__wasilibc_printscan_floating_point_support_option="\"remove -lc-printscan-no-floating-point from the link command\""

$(OBJDIR)/%.long-double.o: $(CURDIR)/%.c $(SYSROOT_INC)
	@mkdir -p "$(@D)"
	"$(WASM_CC)" $(WASM_CFLAGS) -MD -MP -o $@ -c $<

$(OBJDIR)/%.no-floating-point.o: $(CURDIR)/%.c $(SYSROOT_INC)
	@mkdir -p "$(@D)"
	"$(WASM_CC)" $(WASM_CFLAGS) -MD -MP -o $@ -c $<

$(OBJDIR)/%.o: $(CURDIR)/%.c $(SYSROOT_INC)
	@mkdir -p "$(@D)"
	"$(WASM_CC)" $(WASM_CFLAGS) -MD -MP -o $@ -c $<

include $(shell find $(OBJDIR) -name \*.d) /dev/null

$(DLMALLOC_OBJS): override WASM_CFLAGS += \
    -I$(DLMALLOC_INC)

$(LIBC_BOTTOM_HALF_ALL_OBJS): override WASM_CFLAGS += \
    -I$(LIBC_BOTTOM_HALF_HEADERS_PRIVATE) \
    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC) \
    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) \
    -I$(LIBC_BOTTOM_HALF_LIBPREOPEN_LIB) \
    -I$(LIBC_BOTTOM_HALF_LIBPREOPEN_INC)

$(LIBC_TOP_HALF_ALL_OBJS) $(MUSL_PRINTSCAN_LONG_DOUBLE_OBJS) $(MUSL_PRINTSCAN_NO_FLOATING_POINT_OBJS): override WASM_CFLAGS += \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/include \
    -I$(LIBC_TOP_HALF_MUSL_SRC_DIR)/internal \
    -I$(LIBC_TOP_HALF_MUSL_DIR)/arch/wasm32 \
    -I$(LIBC_TOP_HALF_MUSL_DIR)/arch/generic \
    -I$(LIBC_TOP_HALF_HEADERS_PRIVATE) \
    -Wno-parentheses \
    -Wno-shift-op-parentheses \
    -Wno-bitwise-op-parentheses \
    -Wno-logical-op-parentheses \
    -Wno-string-plus-int \
    -Wno-dangling-else \
    -Wno-unknown-pragmas

$(SYSROOT): startup_files libc finish check

$(SYSROOT_INC):
	$(RM) -r "$(SYSROOT)"

	#
	# Install the include files.
	#
	mkdir -p "$(SYSROOT_INC)"
	cp -r "$(BASICS_INC)" "$(SYSROOT)"
	cp -r "$(LIBC_BOTTOM_HALF_HEADERS_PUBLIC)"/* "$(SYSROOT_INC)"

	# Generate musl's bits/alltypes.h header.
	mkdir -p "$(SYSROOT_INC)/bits"
	sed -f $(LIBC_TOP_HALF_MUSL_DIR)/tools/mkalltypes.sed \
	    $(LIBC_TOP_HALF_MUSL_DIR)/arch/wasm32/bits/alltypes.h.in \
	    $(LIBC_TOP_HALF_MUSL_DIR)/include/alltypes.h.in \
	    > "$(SYSROOT_INC)/bits/alltypes.h"

	# Copy in the bulk of musl's public header files.
	cp -r "$(LIBC_TOP_HALF_MUSL_INC)"/* "$(SYSROOT_INC)"
	# Copy in the musl's "bits" header files.
	cp -r "$(LIBC_TOP_HALF_MUSL_DIR)"/arch/generic/bits/* "$(SYSROOT_INC)/bits"
	cp -r "$(LIBC_TOP_HALF_MUSL_DIR)"/arch/wasm32/bits/* "$(SYSROOT_INC)/bits"

	# Remove files that aren't headers or that aren't supported yet or that aren't relevant for wasm.
	$(RM) "$(SYSROOT_INC)/bits/syscall.h.in" \
	      "$(SYSROOT_INC)/bits/alltypes.h.in" \
	      "$(SYSROOT_INC)/alltypes.h.in" \
	      "$(SYSROOT_INC)/sys/procfs.h" \
	      "$(SYSROOT_INC)/sys/user.h" \
	      "$(SYSROOT_INC)/sys/kd.h" \
	      "$(SYSROOT_INC)/bits/kd.h" \
	      "$(SYSROOT_INC)/sys/vt.h" \
	      "$(SYSROOT_INC)/bits/vt.h" \
	      "$(SYSROOT_INC)/sys/soundcard.h" \
	      "$(SYSROOT_INC)/bits/soundcard.h" \
	      "$(SYSROOT_INC)/sys/sem.h" \
	      "$(SYSROOT_INC)/bits/sem.h" \
	      "$(SYSROOT_INC)/sys/statfs.h" \
	      "$(SYSROOT_INC)/sys/vfs.h" \
	      "$(SYSROOT_INC)/bits/statfs.h" \
	      "$(SYSROOT_INC)/sys/statvfs.h" \
	      "$(SYSROOT_INC)/sys/shm.h" \
	      "$(SYSROOT_INC)/bits/shm.h" \
	      "$(SYSROOT_INC)/sys/msg.h" \
	      "$(SYSROOT_INC)/bits/msg.h" \
	      "$(SYSROOT_INC)/sys/ipc.h" \
	      "$(SYSROOT_INC)/bits/ipc.h" \
	      "$(SYSROOT_INC)/syslog.h" \
	      "$(SYSROOT_INC)/sys/syslog.h" \
	      "$(SYSROOT_INC)/paths.h" \
	      "$(SYSROOT_INC)/utmp.h" \
	      "$(SYSROOT_INC)/utmpx.h" \
	      "$(SYSROOT_INC)/lastlog.h" \
	      "$(SYSROOT_INC)/sys/acct.h" \
	      "$(SYSROOT_INC)/sys/cachectl.h" \
	      "$(SYSROOT_INC)/sys/epoll.h" \
	      "$(SYSROOT_INC)/sys/ptrace.h" \
	      "$(SYSROOT_INC)/bits/ptrace.h" \
	      "$(SYSROOT_INC)/sys/reboot.h" \
	      "$(SYSROOT_INC)/sys/swap.h" \
	      "$(SYSROOT_INC)/sys/sendfile.h" \
	      "$(SYSROOT_INC)/sys/quota.h" \
	      "$(SYSROOT_INC)/sys/inotify.h" \
	      "$(SYSROOT_INC)/sys/klog.h" \
	      "$(SYSROOT_INC)/sys/fsuid.h" \
	      "$(SYSROOT_INC)/sys/io.h" \
	      "$(SYSROOT_INC)/sys/prctl.h" \
	      "$(SYSROOT_INC)/sys/mtio.h" \
	      "$(SYSROOT_INC)/sys/mount.h" \
	      "$(SYSROOT_INC)/sys/fanotify.h" \
	      "$(SYSROOT_INC)/sys/personality.h" \
	      "$(SYSROOT_INC)/sys/wait.h" \
	      "$(SYSROOT_INC)/wait.h" \
	      "$(SYSROOT_INC)/bits/errno.h" \
	      "$(SYSROOT_INC)/bits/link.h" \
	      "$(SYSROOT_INC)/link.h" \
	      "$(SYSROOT_INC)/elf.h" \
	      "$(SYSROOT_INC)/scsi/scsi.h" \
	      "$(SYSROOT_INC)/scsi/scsi_ioctl.h" \
	      "$(SYSROOT_INC)/scsi/sg.h" \
	      "$(SYSROOT_INC)/sys/auxv.h" \
	      "$(SYSROOT_INC)/setjmp.h" \
	      "$(SYSROOT_INC)/pwd.h" \
	      "$(SYSROOT_INC)/shadow.h" \
	      "$(SYSROOT_INC)/grp.h" \
	      "$(SYSROOT_INC)/mntent.h" \
	      "$(SYSROOT_INC)/netdb.h" \
	      "$(SYSROOT_INC)/resolv.h" \
	      "$(SYSROOT_INC)/pty.h" \
	      "$(SYSROOT_INC)/dlfcn.h" \
	      "$(SYSROOT_INC)/ulimit.h" \
	      "$(SYSROOT_INC)/xattr.h" \
	      "$(SYSROOT_INC)/wordexp.h" \
	      "$(SYSROOT_INC)/spawn.h" \
	      "$(SYSROOT_INC)/ucontext.h" \
	      "$(SYSROOT_INC)/sys/ucontext.h" \
	      "$(SYSROOT_INC)/sys/membarrier.h"
ifeq ($(THREAD_MODEL), single)
	$(RM) "$(SYSROOT_INC)/aio.h" \
	      "$(SYSROOT_INC)/pthread.h"
endif

ifeq ($(BUILD_LIBC_BOTTOM_HALF),no)
override CRT_SOURCES = $(BASICS_CRT_SOURCES)
else
override CRT_SOURCES = $(LIBC_BOTTOM_HALF_CRT_SOURCES)
endif

startup_files: $(SYSROOT_INC)
	#
	# Build the startup files.
	#
	@mkdir -p "$(OBJDIR)"
	cd "$(OBJDIR)" && \
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(CRT_SOURCES) -MD -MP && \
	mkdir -p "$(SYSROOT_LIB)" && \
	mv *.o "$(SYSROOT_LIB)"

libc: $(SYSROOT_INC) \
    $(SYSROOT_LIB)/libc.a \
    $(SYSROOT_LIB)/libc-printscan-long-double.a \
    $(SYSROOT_LIB)/libc-printscan-no-floating-point.a \
    $(SYSROOT_LIB)/libwasi-emulated-mman.a

finish: $(SYSROOT_INC) libc
	#
	# Create empty placeholder libraries.
	#
	for name in m rt pthread crypt util xnet resolv dl c++ c++abi; do \
	    $(WASM_AR) crs "$(SYSROOT_LIB)/lib$${name}.a"; \
	done

	#
	# Collect metadata on the sysroot and perform sanity checks.
	#
	mkdir -p "$(SYSROOT_SHARE)"

	# Collect symbol information.
	# TODO: Use llvm-nm --extern-only instead of grep. This is blocked on
	# LLVM PR40497, which is fixed in 9.0, but not in 8.0.
	$(WASM_NM) --defined-only "$(SYSROOT_LIB)"/libc.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' [[:upper:]] ' |sed 's/.* [[:upper:]] //' |LC_COLLATE=C sort > "$(SYSROOT_SHARE)/defined-symbols.txt"
	for undef_sym in $$($(WASM_NM) --undefined-only "$(SYSROOT_LIB)"/*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' U ' |sed 's/.* U //' |LC_COLLATE=C sort |uniq); do \
	    grep -q '\<'$$undef_sym'\>' "$(SYSROOT_SHARE)/defined-symbols.txt" || echo $$undef_sym; \
	done > "$(SYSROOT_SHARE)/undefined-symbols.txt"
	grep '^_*wasi_' "$(SYSROOT_SHARE)/undefined-symbols.txt" \
	    > "$(SYSROOT_LIB)/libc.imports"

	# Generate a test file that includes all public header files.
	cd "$(SYSROOT)" && \
	for header in $$(find include -type f -not -name mman.h |grep -v /bits/); do \
	    echo '#include <'$$header'>' | sed 's/include\///' ; \
	done |LC_COLLATE=C sort >share/$(MULTIARCH_TRIPLE)/include-all.c ; \
	cd - >/dev/null

	# Test that it compiles.
	"$(WASM_CC)" $(WASM_CFLAGS) -fsyntax-only "$(SYSROOT_SHARE)/include-all.c" -Wno-\#warnings

	# Collect all the predefined macros, except for compiler version macros
	# which we don't need to track here. For the __*_ATOMIC_*_LOCK_FREE
	# macros, squash individual compiler names to attempt, toward keeping
	# these files compiler-independent.
	# TODO: Undefine __FLOAT128__ for now since it's not in clang 8.0.
	# TODO: Filter out __FLT16_* for now, as not all versions of clang have these.
	"$(WASM_CC)" $(WASM_CFLAGS) "$(SYSROOT_SHARE)/include-all.c" \
	    -E -dM -Wno-\#warnings \
	    -D_ALL_SOURCE \
	    -U__llvm__ \
	    -U__clang__ \
	    -U__clang_major__ \
	    -U__clang_minor__ \
	    -U__clang_patchlevel__ \
	    -U__clang_version__ \
	    -U__GNUC__ \
	    -U__GNUC_MINOR__ \
	    -U__GNUC_PATCHLEVEL__ \
	    -U__VERSION__ \
	    -U__FLOAT128__ \
	    | sed -e 's/__[[:upper:][:digit:]]*_ATOMIC_\([[:upper:][:digit:]_]*\)_LOCK_FREE/__compiler_ATOMIC_\1_LOCK_FREE/' \
	    | grep -v '^#define __FLT16_' \
	    > "$(SYSROOT_SHARE)/predefined-macros.txt"

	#
	# The build succeeded! The generated sysroot is in $(SYSROOT).
	#

check: $(SYSROOT) finish
	# Check that the computed metadata matches the expected metadata.
	diff -ur "$(CURDIR)/expected/$(MULTIARCH_TRIPLE)" "$(SYSROOT_SHARE)"

install: $(SYSROOT)
	mkdir -p "$(INSTALL_DIR)"
	cp -r "$(SYSROOT)/lib" "$(SYSROOT)/share" "$(SYSROOT)/include" "$(INSTALL_DIR)"

.PHONY: $(SYSROOT) default startup_files libc finish check install $(SYSROOT_INC)
