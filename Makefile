# These variables are specifically meant to be overridable via
# the make command-line.
WASM_CC = clang
WASM_CFLAGS = -O2
WASM_TARGET_FLAGS = --target=wasm32
SYSROOT = sysroot

# These variables describe the locations of various files and
# directories in the source tree.
BASICS_DIR = basics
BASICS_INC = basics/include
BASICS_LIBC_DIR = $(BASICS_DIR)/libc
BASICS_LIBC_SOURCES = $(BASICS_LIBC_DIR)/string.c

.PHONY: $(SYSROOT)
$(SYSROOT):
	$(RM) -r "$(SYSROOT)"
	mkdir -p "$(SYSROOT)/usr"

	#
	# Install the include files.
	#
	cp -r  "$(BASICS_INC)" "$(SYSROOT)/usr"

	#
	# Build the C startup files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(BASICS_LIBC_DIR)/crt*.s
	mkdir -p "$(SYSROOT)/lib"
	mv *.o "$(SYSROOT)/lib"

	#
	# Compile and install the libc-subset source files.
	#
	$(RM) *.o
	"$(WASM_CC)" $(WASM_CFLAGS) $(WASM_TARGET_FLAGS) --sysroot="$(SYSROOT)" -c $(BASICS_LIBC_SOURCES)
	mkdir -p "$(SYSROOT)/lib"
	$(AR) crs "$(SYSROOT)/lib/libc.a" *.o
	$(RM) *.o
