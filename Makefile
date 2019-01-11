# These variables are specifically meant to be overridable via
# the make command-line.
WASM_CC = clang
WASM_NM = $(subst clang,llvm-nm,$(WASM_CC))
WASM_AR = $(subst clang,llvm-ar,$(WASM_CC))
WASM_CFLAGS = -O2
WASM_TARGET_FLAGS = --target=wasm32
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
COWS_LIBC_DIR = cows-libc
COWS_LIBC_CLOUDLIBC_SRC = $(COWS_LIBC_DIR)/cloudlibc/src
COWS_LIBC_CLOUDLIBC_SRC_INC = $(COWS_LIBC_CLOUDLIBC_SRC)/include
COWS_LIBC_HEADERS_PUBLIC = $(COWS_LIBC_DIR)/headers/public
COWS_LIBC_HEADERS_PRIVATE = $(COWS_LIBC_DIR)/headers/private
COWS_LIBC_CLOUDABI_HEADERS = $(COWS_LIBC_DIR)/cloudabi/headers
COWS_LIBC_LIBPREOPEN_DIR = $(COWS_LIBC_DIR)/libpreopen
COWS_LIBC_LIBPREOPEN_LIB = $(COWS_LIBC_LIBPREOPEN_DIR)/lib
COWS_LIBC_LIBPREOPEN_INC = $(COWS_LIBC_LIBPREOPEN_DIR)/include
COWS_LIBC_SOURCES = $(COWS_LIBC_DIR)/sources
COWS_LIBC_ALL_SOURCES = \
    $(shell find $(COWS_LIBC_CLOUDLIBC_SRC) -name \*.c) \
    $(COWS_LIBC_LIBPREOPEN_LIB)/po_libc_wrappers.c \
    $(shell find $(COWS_LIBC_SOURCES) -name \*.c)
MUSL_LIBC_DIR = musl-libc/musl
MUSL_LIBC_SRC_DIR = $(MUSL_LIBC_DIR)/src
MUSL_LIBC_INC = $(MUSL_LIBC_DIR)/include
# TODO: Add more sources.
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
    $(MUSL_LIBC_SRC_DIR)/stdio/ofl.c \
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
    $(MUSL_LIBC_SRC_DIR)/misc/getauxval.c \
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

# These variables describe the locations of various files and
# directories in the generated sysroot tree.
SYSROOT_LIB = $(SYSROOT)/lib32
SYSROOT_INC = $(SYSROOT)/include
SYSROOT_SHARE = $(SYSROOT)/share

# Configure support for threads.
ifeq ($(THREAD_MODEL), single)
WASM_CFLAGS += -mthread-model single -D__WASM_THREAD_MODEL_SINGLE
endif
ifeq ($(THREAD_MODEL), posix)
WASM_CFLAGS += -mthread-model posix -pthread -D__WASM_THREAD_MODEL_POSIX
endif

.PHONY: $(SYSROOT)
$(SYSROOT):
	$(RM) -r "$(SYSROOT)"
	mkdir -p "$(SYSROOT)"

	#
	# Install the include files.
	#
	cp -r --backup=numbered "$(BASICS_INC)" "$(SYSROOT)"

	#
	# Build the C startup files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" \
	    -c $(BASICS_LIBC_DIR)/crt*.S
	mkdir -p "$(SYSROOT_LIB)"
	mv *.o "$(SYSROOT_LIB)"

	#
	# Compile and install the basics libc subset source files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" \
	    -c $(BASICS_LIBC_SOURCES)
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-basics.a" *.o
	$(RM) *.o

	#
	# Compile and install the dlmalloc source files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" \
	    -c $(DLMALLOC_SOURCES) -I$(DLMALLOC_INC)
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-dlmalloc.a" *.o
	$(RM) *.o

	#
	# Compile and install the COWS libc source files.
	#
	$(RM) *.o
	cp -r --backup=numbered "$(COWS_LIBC_CLOUDABI_HEADERS)"/* "$(SYSROOT_INC)"
	cp -r --backup=numbered "$(COWS_LIBC_HEADERS_PUBLIC)"/* "$(SYSROOT_INC)"
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c \
	    $(COWS_LIBC_ALL_SOURCES) \
	    -I$(COWS_LIBC_HEADERS_PRIVATE) \
	    -I$(COWS_LIBC_CLOUDLIBC_SRC_INC) -I$(COWS_LIBC_CLOUDLIBC_SRC) \
	    -I$(COWS_LIBC_LIBPREOPEN_LIB) -I$(COWS_LIBC_LIBPREOPEN_INC)
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-cows.a" *.o
	$(RM) *.o

	#
	# Compile and install musl source files.
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
	# Remove files that aren't headers or that aren't supported or relevant for wasm.
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
	      "$(SYSROOT_INC)/bits/errno.h"
	# Compile the musl sources.
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c \
	    $(MUSL_LIBC_SOURCES) \
	    -I $(MUSL_LIBC_SRC_DIR)/include \
	    -I $(MUSL_LIBC_SRC_DIR)/internal \
	    -I $(MUSL_LIBC_DIR)/arch/wasm32 \
	    -I $(MUSL_LIBC_DIR)/arch/generic \
	    -Wno-shift-op-parentheses \
	    -Wno-string-plus-int \
	    -Wno-unknown-pragmas \
	    -Wno-dangling-else
	# Create the library.
	mkdir -p "$(SYSROOT_LIB)"
	$(WASM_AR) crs "$(SYSROOT_LIB)/libc-musl.a" *.o
	$(RM) *.o

	#
	# Collect metadata on the sysroot and perform sanity checks.
	#
	mkdir -p "$(SYSROOT_SHARE)"

	# Collect symbol information.
	# FIXME: --extern-only doesn't seem to report all the defined symbols.
	$(WASM_NM) --defined-only "$(SYSROOT_LIB)"/*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' [[:upper:]] ' |sed 's/.* [[:upper:]] //' |sort > "$(SYSROOT_SHARE)/wasm32-defined-symbols.txt"
	for undef_sym in $$($(WASM_NM) --undefined-only "$(SYSROOT_LIB)"/*.a "$(SYSROOT_LIB)"/*.o \
	    |grep ' U ' |sed 's/.* U //' |sort |uniq); do \
	    grep -q $$undef_sym "$(SYSROOT_SHARE)/wasm32-defined-symbols.txt" || echo $$undef_sym; \
	done > "$(SYSROOT_SHARE)/wasm32-undefined-symbols.txt"

	# Generate a test file that includes all public header files.
	cd "$(SYSROOT)" && \
	for header in $$(find include -type f |grep -v /bits/); do \
	    echo '#include <'$$header'>' | sed 's/include\///' >> share/wasm32-include-all.c; \
	done; \
	cd - >/dev/null

	# Test that it compiles.
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" \
	    -fsyntax-only "$(SYSROOT_SHARE)/wasm32-include-all.c" -Weverything -Wno-\#warnings

	# Collect all the predefined macros.
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" \
	    -E -dM "$(SYSROOT_SHARE)/wasm32-include-all.c" -Wno-\#warnings > "$(SYSROOT_SHARE)/wasm32-predefined-macros.txt"

	# Remove this once https://reviews.llvm.org/D56553 is resolved.
	ln -s lib32 $(SYSROOT)/lib

	# Check that the computed metadata matches the expected metadata.
	diff -ur expected "$(SYSROOT_SHARE)"

	# The build succeeded!
