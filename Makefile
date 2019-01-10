# These variables are specifically meant to be overridable via
# the make command-line.
WASM_CC = clang
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
    $(MUSL_LIBC_SRC_DIR)/stdio/*printf*.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/*scanf*.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fwrite.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fread.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fputs.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/puts.c \
    $(MUSL_LIBC_SRC_DIR)/stdio/fgets.c \
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
    $(MUSL_LIBC_SRC_DIR)/math/*.c \
    $(MUSL_LIBC_SRC_DIR)/complex/*.c \
    $(MUSL_LIBC_SRC_DIR)/crypt/*.c \
    $(MUSL_LIBC_SRC_DIR)/exit/exit.c

# Configure support for threads.
ifeq ($(THREAD_MODEL), single)
WASM_CFLAGS += -mthread-model single -D__WASM_THREAD_MODEL_SINGLE
endif
ifeq ($(THREAD_MODEL), posix)
WASM_CFLAGS += -mthread-model posix -D__WASM_THREAD_MODEL_POSIX
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
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(BASICS_LIBC_DIR)/crt*.S
	mkdir -p "$(SYSROOT)/lib32"
	mv *.o "$(SYSROOT)/lib32"

	#
	# Compile and install the basics libc subset source files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(BASICS_LIBC_SOURCES)
	mkdir -p "$(SYSROOT)/lib32"
	$(AR) crs "$(SYSROOT)/lib32/libc-basics.a" *.o
	$(RM) *.o

	#
	# Compile and install the dlmalloc source files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(DLMALLOC_SOURCES) -I$(DLMALLOC_INC)
	mkdir -p "$(SYSROOT)/lib32"
	$(AR) crs "$(SYSROOT)/lib32/libc-dlmalloc.a" *.o
	$(RM) *.o

	#
	# Compile and install the COWS source files.
	#
	$(RM) *.o
	cp -r --backup=numbered "$(COWS_LIBC_CLOUDABI_HEADERS)"/* "$(SYSROOT)/include"
	cp -r --backup=numbered "$(COWS_LIBC_HEADERS_PUBLIC)"/* "$(SYSROOT)/include"
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(COWS_LIBC_ALL_SOURCES) \
	    -I$(COWS_LIBC_HEADERS_PRIVATE) \
	    -I$(COWS_LIBC_CLOUDLIBC_SRC_INC) -I$(COWS_LIBC_CLOUDLIBC_SRC) \
	    -I$(COWS_LIBC_LIBPREOPEN_LIB) -I$(COWS_LIBC_LIBPREOPEN_INC)
	mkdir -p "$(SYSROOT)/lib32"
	$(AR) crs "$(SYSROOT)/lib32/libc-cows.a" *.o
	$(RM) *.o

	#
	# Compile and install musl source files.
	#
	mkdir -p "$(SYSROOT)/include/bits"
	sed -f $(MUSL_LIBC_DIR)/tools/mkalltypes.sed \
	    $(MUSL_LIBC_DIR)/arch/wasm32/bits/alltypes.h.in \
	    $(MUSL_LIBC_DIR)/include/alltypes.h.in \
	    > $(SYSROOT)/include/bits/alltypes.h
	cp -r --backup=numbered "$(MUSL_LIBC_INC)"/* "$(SYSROOT)/include"
	$(RM) include/bits/syscall.h.in \
	      include/bits/alltypes.h.in \
	      include/alltypes.h.in
	mkdir -p $(SYSROOT)/include/bits
	:> $(SYSROOT)/include/bits/syscall.h
	cp -r --backup=numbered "$(MUSL_LIBC_DIR)"/arch/generic/bits/* "$(SYSROOT)/include/bits"
	cp -r --backup=numbered "$(MUSL_LIBC_DIR)"/arch/wasm32/bits/* "$(SYSROOT)/include/bits"
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(MUSL_LIBC_SOURCES) \
	    -I $(MUSL_LIBC_SRC_DIR)/include \
	    -I $(MUSL_LIBC_SRC_DIR)/internal \
	    -I $(MUSL_LIBC_DIR)/arch/wasm32 \
	    -I $(MUSL_LIBC_DIR)/arch/generic
	mkdir -p "$(SYSROOT)/lib32"
	$(AR) crs "$(SYSROOT)/lib32/libc-musl.a" *.o
	$(RM) *.o
