# These variables are specifically meant to be overridable via
# the make command-line.
WASM_CC = clang
WASM_NM = $(subst clang,llvm-nm,$(WASM_CC))
WASM_AR = $(subst clang,llvm-ar,$(WASM_CC))
WASM_CFLAGS = -O2
SYSROOT = sysroot
# single or posix
THREAD_MODEL = single

# These variables describe the locations of various files and
# directories in the source tree.
BASICS_DIR = basics
BASICS_INC = $(BASICS_DIR)/include
BASICS_LIBC_DIR = $(BASICS_DIR)/libc
BASICS_LIBC_SOURCES = $(BASICS_LIBC_DIR)/string.c
DLMALLOC_DIR = dlmalloc
DLMALLOC_SRC_DIR = $(DLMALLOC_DIR)/src
DLMALLOC_SOURCES = $(DLMALLOC_SRC_DIR)/wrapper.c
DLMALLOC_INC = $(DLMALLOC_DIR)/include
LIBC_BOTTOM_HALF_DIR = libc-bottom-half
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC = $(LIBC_BOTTOM_HALF_DIR)/cloudlibc/src
LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC = $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC)/include
LIBC_BOTTOM_HALF_HEADERS_PUBLIC = $(LIBC_BOTTOM_HALF_DIR)/headers/public
LIBC_BOTTOM_HALF_HEADERS_PRIVATE = $(LIBC_BOTTOM_HALF_DIR)/headers/private
LIBC_BOTTOM_HALF_CLOUDABI_HEADERS = $(LIBC_BOTTOM_HALF_DIR)/cloudabi/headers
LIBC_BOTTOM_HALF_LIBPREOPEN_DIR = $(LIBC_BOTTOM_HALF_DIR)/libpreopen
LIBC_BOTTOM_HALF_LIBPREOPEN_LIB = $(LIBC_BOTTOM_HALF_LIBPREOPEN_DIR)/lib
LIBC_BOTTOM_HALF_LIBPREOPEN_INC = $(LIBC_BOTTOM_HALF_LIBPREOPEN_DIR)/include
LIBC_BOTTOM_HALF_SOURCES = $(LIBC_BOTTOM_HALF_DIR)/sources
LIBC_BOTTOM_HALF_ALL_SOURCES = \
    $(shell find $(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) -name \*.c) \
    $(LIBC_BOTTOM_HALF_LIBPREOPEN_LIB)/po_libc_wrappers.c \
    $(shell find $(LIBC_BOTTOM_HALF_SOURCES) -name \*.c)
LIBC_TOP_HALF_DIR = libc-top-half
MUSL_LIBC_DIR = $(LIBC_TOP_HALF_DIR)/musl
MUSL_LIBC_SRC_DIR = $(MUSL_LIBC_DIR)/src
MUSL_LIBC_INC = $(MUSL_LIBC_DIR)/include
MUSL_LIBC_SOURCES = \
    $(MUSL_LIBC_SRC_DIR)/stdio/asprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/dprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/printf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/snprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/sprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vasprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vdprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vfprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vsnprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vsprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/scanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/sscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vfscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vsscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fwrite.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fread.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fputs.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/puts.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/putc.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fgets.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/getc.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fopen.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fclose.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/stdin.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/stdout.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/stderr.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fflush.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/feof.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/ferror.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/clearerr.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__overflow.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__towrite.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdio_exit.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdio_close.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdio_read.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdio_write.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdio_seek.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__stdout_write.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/__fmodeflags.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/ofl.c \
    $(MUSL_LIBC_SRC_DIR)/internal/floatscan.c \
    $(MUSL_LIBC_SRC_DIR)/string/bcmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/bcopy.c \
    $(MUSL_LIBC_SRC_DIR)/string/bzero.c \
    $(MUSL_LIBC_SRC_DIR)/string/explicit_bzero.c \
    $(MUSL_LIBC_SRC_DIR)/string/index.c \
    $(MUSL_LIBC_SRC_DIR)/string/memccpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/memchr.c \
    $(MUSL_LIBC_SRC_DIR)/string/memcmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/memcpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/memmem.c \
    $(MUSL_LIBC_SRC_DIR)/string/memmove.c \
    $(MUSL_LIBC_SRC_DIR)/string/mempcpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/memrchr.c \
    $(MUSL_LIBC_SRC_DIR)/string/memset.c \
    $(MUSL_LIBC_SRC_DIR)/string/rindex.c \
    $(MUSL_LIBC_SRC_DIR)/string/stpcpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/stpncpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcasecmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcasestr.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcat.c \
    $(MUSL_LIBC_SRC_DIR)/string/strchr.c \
    $(MUSL_LIBC_SRC_DIR)/string/strchrnul.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/strcspn.c \
    $(MUSL_LIBC_SRC_DIR)/string/strdup.c \
    $(MUSL_LIBC_SRC_DIR)/string/strerror_r.c \
    $(MUSL_LIBC_SRC_DIR)/string/strlcat.c \
    $(MUSL_LIBC_SRC_DIR)/string/strlcpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/strlen.c \
    $(MUSL_LIBC_SRC_DIR)/string/strncasecmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/strncat.c \
    $(MUSL_LIBC_SRC_DIR)/string/strncmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/strncpy.c \
    $(MUSL_LIBC_SRC_DIR)/string/strndup.c \
    $(MUSL_LIBC_SRC_DIR)/string/strnlen.c \
    $(MUSL_LIBC_SRC_DIR)/string/strpbrk.c \
    $(MUSL_LIBC_SRC_DIR)/string/strrchr.c \
    $(MUSL_LIBC_SRC_DIR)/string/strsep.c \
    $(MUSL_LIBC_SRC_DIR)/string/strspn.c \
    $(MUSL_LIBC_SRC_DIR)/string/strstr.c \
    $(MUSL_LIBC_SRC_DIR)/string/strtok.c \
    $(MUSL_LIBC_SRC_DIR)/string/strtok_r.c \
    $(MUSL_LIBC_SRC_DIR)/string/strverscmp.c \
    $(MUSL_LIBC_SRC_DIR)/string/swab.c \
    $(MUSL_LIBC_SRC_DIR)/string/w*.c \
    $(MUSL_LIBC_SRC_DIR)/stdlib/*.c \
    $(MUSL_LIBC_SRC_DIR)/search/*.c \
    $(MUSL_LIBC_SRC_DIR)/termios/*.c \
    $(MUSL_LIBC_SRC_DIR)/temp/*.c \
    $(MUSL_LIBC_SRC_DIR)/prng/*.c \
    $(MUSL_LIBC_SRC_DIR)/misc/a64l.c \
    $(MUSL_LIBC_SRC_DIR)/misc/basename.c \
    $(MUSL_LIBC_SRC_DIR)/misc/dirname.c \
    $(MUSL_LIBC_SRC_DIR)/misc/ffs.c \
    $(MUSL_LIBC_SRC_DIR)/misc/ffsl.c \
    $(MUSL_LIBC_SRC_DIR)/misc/ffsll.c \
    $(MUSL_LIBC_SRC_DIR)/misc/fmtmsg.c \
    $(MUSL_LIBC_SRC_DIR)/misc/get_current_dir_name.c \
    $(MUSL_LIBC_SRC_DIR)/misc/getdomainname.c \
    $(MUSL_LIBC_SRC_DIR)/misc/gethostid.c \
    $(MUSL_LIBC_SRC_DIR)/misc/getopt.c \
    $(MUSL_LIBC_SRC_DIR)/misc/getopt_long.c \
    $(MUSL_LIBC_SRC_DIR)/misc/getsubopt.c \
    $(MUSL_LIBC_SRC_DIR)/misc/login_tty.c \
    $(MUSL_LIBC_SRC_DIR)/misc/syslog.c \
    $(MUSL_LIBC_SRC_DIR)/misc/getentropy.c \
    $(MUSL_LIBC_SRC_DIR)/conf/*.c \
    $(MUSL_LIBC_SRC_DIR)/ctype/*.c \
    $(MUSL_LIBC_SRC_DIR)/math/*.c \
    $(MUSL_LIBC_SRC_DIR)/complex/*.c \
    $(MUSL_LIBC_SRC_DIR)/crypt/*.c \
    $(MUSL_LIBC_SRC_DIR)/errno/strerror.c \
    $(MUSL_LIBC_SRC_DIR)/fenv/fenv.c \
    $(MUSL_LIBC_SRC_DIR)/exit/exit.c \
    $(MUSL_LIBC_SRC_DIR)/exit/atexit.c
MUSL_PRINTSCAN_SOURCES = \
    $(MUSL_LIBC_SRC_DIR)/internal/floatscan.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vfprintf.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/vfscanf.c \
    $(MUSL_LIBC_SRC_DIR)/stdlib/strtod.c
LIBC_TOP_HALF_HEADERS_PRIVATE = $(LIBC_TOP_HALF_DIR)/headers/private

# These variables describe the locations of various files and
# directories in the generated sysroot tree.
MULTIARCH_TRIPLE = wasm32-wasi-musl
SYSROOT_LIB = $(SYSROOT)/lib/$(MULTIARCH_TRIPLE)
SYSROOT_INC = $(SYSROOT)/include
SYSROOT_SHARE = $(SYSROOT)/share/$(MULTIARCH_TRIPLE)

# Set the target.
# TODO: Add -unknown-wasi-musl when the compiler supports it.
override WASM_CFLAGS += --target=wasm32-unknown-wasi-musl
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

.PHONY: $(SYSROOT)
$(SYSROOT):
	$(RM) -r "$(SYSROOT)"
	mkdir -p "$(SYSROOT)"

	#
	# Install the basics include files.
	#
	cp -r --backup=numbered "$(BASICS_INC)" "$(SYSROOT)"

	#
	# Build the C startup files.
	#
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(BASICS_LIBC_DIR)/crt*.c
	mkdir -p "$(SYSROOT_LIB)"
	mv *.o "$(SYSROOT_LIB)"

	#
	# Compile the basics libc subset source files.
	#
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(BASICS_LIBC_SOURCES)

	#
	# Compile the dlmalloc source files.
	#
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(DLMALLOC_SOURCES) -I$(DLMALLOC_INC)

	#
	# Compile the WASI libc source files.
	#
	cp -r --backup=numbered "$(LIBC_BOTTOM_HALF_CLOUDABI_HEADERS)"/* "$(SYSROOT_INC)"
	cp -r --backup=numbered "$(LIBC_BOTTOM_HALF_HEADERS_PUBLIC)"/* "$(SYSROOT_INC)"
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(LIBC_BOTTOM_HALF_ALL_SOURCES) \
	    -I$(LIBC_BOTTOM_HALF_HEADERS_PRIVATE) \
	    -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC) -I$(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC) \
	    -I$(LIBC_BOTTOM_HALF_LIBPREOPEN_LIB) -I$(LIBC_BOTTOM_HALF_LIBPREOPEN_INC)

	#
	# Compile the musl libc source files.
	#
	mkdir -p "$(SYSROOT_INC)/bits"
	# Generate musl's bits/alltypes.h header.
	sed -f $(MUSL_LIBC_DIR)/tools/mkalltypes.sed \
	    $(MUSL_LIBC_DIR)/arch/wasm32/bits/alltypes.h.in \
	    $(MUSL_LIBC_DIR)/include/alltypes.h.in \
	    > "$(SYSROOT_INC)/bits/alltypes.h"
	# Copy in the bulk of musl's public header files.
	cp -r --backup=numbered "$(MUSL_LIBC_INC)"/* "$(SYSROOT_INC)"
	# Copy in the musl's "bits" header files.
	cp -r --backup=numbered "$(MUSL_LIBC_DIR)"/arch/generic/bits/* "$(SYSROOT_INC)/bits"
	cp -r "$(MUSL_LIBC_DIR)"/arch/wasm32/bits/* "$(SYSROOT_INC)/bits"
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
	      "$(SYSROOT_INC)/sys/shm.h" \
	      "$(SYSROOT_INC)/bits/shm.h" \
	      "$(SYSROOT_INC)/sys/msg.h" \
	      "$(SYSROOT_INC)/bits/msg.h" \
	      "$(SYSROOT_INC)/sys/ipc.h" \
	      "$(SYSROOT_INC)/bits/ipc.h" \
	      "$(SYSROOT_INC)/bits/errno.h" \
	      "$(SYSROOT_INC)/bits/link.h" \
	      "$(SYSROOT_INC)/link.h" \
	      "$(SYSROOT_INC)/elf.h" \
	      "$(SYSROOT_INC)/sys/auxv.h"
	# Compile the musl libc sources (long double print/scan disabled).
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(MUSL_LIBC_SOURCES) \
	    -I $(MUSL_LIBC_SRC_DIR)/include \
	    -I $(MUSL_LIBC_SRC_DIR)/internal \
	    -I $(MUSL_LIBC_DIR)/arch/wasm32 \
	    -I $(MUSL_LIBC_DIR)/arch/generic \
	    -I $(LIBC_TOP_HALF_HEADERS_PRIVATE) \
	    -D__wasilibc_printscan_no_long_double \
	    -D__wasilibc_printscan_full_support_option="\"add -lc-printscan-long-double to the link command\"" \
	    -Wno-shift-op-parentheses \
	    -Wno-string-plus-int \
	    -Wno-dangling-else \
	    -Wno-unknown-pragmas

	# Create the musl libc library.
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc.a" *.o
	$(RM) *.o

	# Compile the musl print/scan libc sources with long double support.
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(MUSL_PRINTSCAN_SOURCES) \
	    -I $(MUSL_LIBC_SRC_DIR)/include \
	    -I $(MUSL_LIBC_SRC_DIR)/internal \
	    -I $(MUSL_LIBC_DIR)/arch/wasm32 \
	    -I $(MUSL_LIBC_DIR)/arch/generic \
	    -I $(LIBC_TOP_HALF_HEADERS_PRIVATE) \
	    -Wno-shift-op-parentheses \
	    -Wno-string-plus-int \
	    -Wno-dangling-else \
	    -Wno-unknown-pragmas

	# Create the musl libc-printscan-long-double library.
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-printscan-long-double.a" *.o
	$(RM) *.o

	# Compile the musl libc sources with floating-point print/scan disabled.
	"$(WASM_CC)" $(WASM_CFLAGS) -c $(MUSL_PRINTSCAN_SOURCES) \
	    -I $(MUSL_LIBC_SRC_DIR)/include \
	    -I $(MUSL_LIBC_SRC_DIR)/internal \
	    -I $(MUSL_LIBC_DIR)/arch/wasm32 \
	    -I $(MUSL_LIBC_DIR)/arch/generic \
	    -I $(LIBC_TOP_HALF_HEADERS_PRIVATE) \
	    -D__wasilibc_printscan_no_floating_point \
	    -D__wasilibc_printscan_floating_point_support_option="\"remove -lc-printscan-no-floating-point from the link command\"" \
	    -Wno-shift-op-parentheses \
	    -Wno-string-plus-int \
	    -Wno-dangling-else \
	    -Wno-unknown-pragmas

	# Create the musl libc-printscan-no-floating-point library.
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-printscan-no-floating-point.a" *.o
	$(RM) *.o

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
	# FIXME: --extern-only doesn't seem to report all the defined symbols.
	$(WASM_NM) --defined-only "$(SYSROOT_LIB)"/libc.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' [[:upper:]] ' |sed 's/.* [[:upper:]] //' |sort > "$(SYSROOT_SHARE)/defined-symbols.txt"
	for undef_sym in $$($(WASM_NM) --undefined-only "$(SYSROOT_LIB)"/*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' U ' |sed 's/.* U //' |sort |uniq); do \
	    grep -q $$undef_sym "$(SYSROOT_SHARE)/defined-symbols.txt" || echo $$undef_sym; \
	done   > "$(SYSROOT_SHARE)/undefined-symbols.txt"
	grep ^cloudabi_sys_ "$(SYSROOT_SHARE)/undefined-symbols.txt" \
	    > "$(SYSROOT_LIB)/libc.imports"

	# Generate a test file that includes all public header files.
	cd "$(SYSROOT)" && \
	for header in $$(find include -type f |grep -v /bits/); do \
	    echo '#include <'$$header'>' | sed 's/include\///' >> share/$(MULTIARCH_TRIPLE)/include-all.c; \
	done; \
	cd - >/dev/null

	# Test that it compiles.
	"$(WASM_CC)" $(WASM_CFLAGS) -fsyntax-only "$(SYSROOT_SHARE)/include-all.c" \
	    -Weverything \
	    -Wno-\#warnings

	# Collect all the predefined macros, except for compiler version macros
	# which we don't need to track here. For the __*_ATOMIC_*_LOCK_FREE
	# macros, squash individual compiler names to attempt, toward keeping
	# these files compiler-independent.
	"$(WASM_CC)" $(WASM_CFLAGS) "$(SYSROOT_SHARE)/include-all.c" \
	    -E -dM -Wno-\#warnings \
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
	    | sed -e 's/__[[:upper:][:digit:]]*_ATOMIC_\([[:upper:][:digit:]_]*\)_LOCK_FREE/__compiler_ATOMIC_\1_LOCK_FREE/' \
	    > "$(SYSROOT_SHARE)/predefined-macros.txt"

	# Check that the computed metadata matches the expected metadata.
	diff -ur expected "$(SYSROOT_SHARE)"

	#
	# The build succeeded! The generated sysroot is in $(SYSROOT).
	#
