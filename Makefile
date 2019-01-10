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
