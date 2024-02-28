# Build the `libc-test` tests as Wasm programs and run them with the selected
# engine. Contributors beware! This Makefile follows the style of the
# `libc-test` Makefile and uses some more exotic features of `make`.
#
# The top-level `test` target is composed of a chain of several phony
# sub-targets:
# - `download`: retrieve the `libc-test` source from a Git `$(MIRROR)`
# - `build`: construct Wasm modules for a subset of the `libc-test` tests
# - `run`: execute the benchmarks with a Wasm `$(ENGINE)` of choice (e.g.,
#   Wasmtime)

test: run

# Unlike the `libc-test` directory, we will output all artifacts to the `build`
# directory (keeping with the `wasi-libc` conventions).
OBJDIR ?= $(CURDIR)/build
DOWNDIR ?= $(CURDIR)/download

TARGET_TRIPLE ?= wasm32-wasi

##### DOWNLOAD #################################################################

LIBC_TEST_URL ?= https://github.com/bytecodealliance/libc-test
LIBC_TEST = $(DOWNDIR)/libc-test
LIBRT_URL ?= https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-16/libclang_rt.builtins-wasm32-wasi-16.0.tar.gz
LIBRT = $(DOWNDIR)/lib/wasi/libclang_rt.builtins-wasm32.a
WASMTIME_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v17.0.0/wasmtime-v17.0.0-x86_64-linux.tar.xz
WASMTIME = $(DOWNDIR)/$(shell basename $(WASMTIME_URL) .tar.xz)/wasmtime
WASM_TOOLS_URL ?= https://github.com/bytecodealliance/wasm-tools/releases/download/wasm-tools-1.0.54/wasm-tools-1.0.54-x86_64-linux.tar.gz
WASM_TOOLS = $(DOWNDIR)/$(shell basename $(WASM_TOOLS_URL) .tar.gz)/wasm-tools
ADAPTER_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v17.0.0/wasi_snapshot_preview1.command.wasm
ADAPTER = $(DOWNDIR)/wasi_snapshot_preview1.command.wasm

TO_DOWNLOAD = $(LIBC_TEST) $(LIBRT) $(WASMTIME)
ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
TO_DOWNLOAD += $(ADAPTER) $(WASM_TOOLS)
endif

download: $(TO_DOWNLOAD)

$(DOWNDIR):
	mkdir -p download

$(LIBC_TEST): | $(DOWNDIR)
	git clone --depth 1 $(LIBC_TEST_URL) $@

# TODO install target to place into...
$(LIBRT): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(LIBRT_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(LIBRT_URL)) --directory=$(DOWNDIR)/

$(WASMTIME): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(WASMTIME_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(WASMTIME_URL)) --directory=$(DOWNDIR)/

$(WASM_TOOLS): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(WASM_TOOLS_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(WASM_TOOLS_URL)) --directory=$(DOWNDIR)/

$(ADAPTER): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(ADAPTER_URL)

clean::
	rm -rf download

##### BUILD ####################################################################

# For now, we list out the tests that we can currently build and run. This is
# heavily focused on the functional tests; in the future it would be good to
# fill out the missing tests and also include some `src/api` and `src/math`
# tests (TODO).
TESTS := \
	$(LIBC_TEST)/src/functional/argv.c \
	$(LIBC_TEST)/src/functional/basename.c \
	$(LIBC_TEST)/src/functional/clocale_mbfuncs.c \
	$(LIBC_TEST)/src/functional/clock_gettime.c \
	$(LIBC_TEST)/src/functional/crypt.c \
	$(LIBC_TEST)/src/functional/dirname.c \
	$(LIBC_TEST)/src/functional/env.c \
	$(LIBC_TEST)/src/functional/fnmatch.c \
	$(LIBC_TEST)/src/functional/iconv_open.c \
	$(LIBC_TEST)/src/functional/mbc.c \
	$(LIBC_TEST)/src/functional/memstream.c \
	$(LIBC_TEST)/src/functional/qsort.c \
	$(LIBC_TEST)/src/functional/random.c \
	$(LIBC_TEST)/src/functional/search_hsearch.c \
	$(LIBC_TEST)/src/functional/search_insque.c \
	$(LIBC_TEST)/src/functional/search_lsearch.c \
	$(LIBC_TEST)/src/functional/search_tsearch.c \
	$(LIBC_TEST)/src/functional/snprintf.c \
	$(LIBC_TEST)/src/functional/sscanf.c \
	$(LIBC_TEST)/src/functional/strftime.c \
	$(LIBC_TEST)/src/functional/string.c \
	$(LIBC_TEST)/src/functional/string_memcpy.c \
	$(LIBC_TEST)/src/functional/string_memmem.c \
	$(LIBC_TEST)/src/functional/string_memset.c \
	$(LIBC_TEST)/src/functional/string_strchr.c \
	$(LIBC_TEST)/src/functional/string_strcspn.c \
	$(LIBC_TEST)/src/functional/string_strstr.c \
	$(LIBC_TEST)/src/functional/strtod.c \
	$(LIBC_TEST)/src/functional/strtod_long.c \
	$(LIBC_TEST)/src/functional/strtod_simple.c \
	$(LIBC_TEST)/src/functional/strtof.c \
	$(LIBC_TEST)/src/functional/strtol.c \
	$(LIBC_TEST)/src/functional/swprintf.c \
	$(LIBC_TEST)/src/functional/tgmath.c \
	$(LIBC_TEST)/src/functional/udiv.c \
	$(LIBC_TEST)/src/functional/wcsstr.c \
	$(LIBC_TEST)/src/functional/wcstol.c

# Part of the problem including more tests is that the `libc-test`
# infrastructure code is not all Wasm-compilable. As we include more tests
# above, this list will also likely need to grow.
COMMON_TEST_INFRA = \
	$(LIBC_TEST)/src/common/path.c \
	$(LIBC_TEST)/src/common/print.c \
	$(LIBC_TEST)/src/common/rand.c \
	$(LIBC_TEST)/src/common/utf8.c

# Create various lists containing the various artifacts to be built: mainly,
# $(WASM_OBJS) are compiled in the $(OBJDIRS) and then linked together to form
# the $(WASMS) tests.
NAMES := $(TESTS:$(LIBC_TEST)/src/%.c=%)
WASMS := $(TESTS:$(LIBC_TEST)/src/%.c=$(OBJDIR)/%.core.wasm)
WASM_OBJS := $(TESTS:$(LIBC_TEST)/src/%.c=$(OBJDIR)/%.wasm.o)
INFRA_WASM_OBJS := $(COMMON_TEST_INFRA:$(LIBC_TEST)/src/%.c=$(OBJDIR)/%.wasm.o)
WASM_OBJS += $(INFRA_WASM_OBJS)
DIRS := $(patsubst $(OBJDIR)/%/,%,$(sort $(dir $(WASM_OBJS))))
OBJDIRS := $(DIRS:%=$(OBJDIR)/%)

# Allow $(CC) to be set from the command line; ?= doesn't work for CC because
# make has a default value for it.
ifeq ($(origin CC), default)
CC := clang
endif
LDFLAGS ?=
CFLAGS ?= --target=$(TARGET_TRIPLE) --sysroot=../sysroot
# Always include the `libc-test` infrastructure headers.
CFLAGS += -I$(LIBC_TEST)/src/common
ifneq ($(findstring -threads,$(TARGET_TRIPLE)),)
CFLAGS += -pthread
endif

# Compile each selected test using Clang. Note that failures here are likely
# due to a missing `libclang_rt.builtins-wasm32.a` in the Clang lib directory.
# This location is system-dependent, but could be fixed by something like:
#  $ sudo mkdir /usr/lib64/clang/14.0.5/lib/wasi
#  $ sudo cp download/lib/wasi/libclang_rt.builtins-wasm32.a /usr/lib64/clang/14.0.5/lib/wasi/
build: download $(WASMS)

$(WASMS): | $(OBJDIRS)
$(OBJDIR)/%.core.wasm: $(OBJDIR)/%.wasm.o $(INFRA_WASM_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
$(OBJDIR)/%.wasm: $(OBJDIR)/%.core.wasm
	$(WASM_TOOLS) component new --adapt $(ADAPTER) $< -o $@
endif

$(WASM_OBJS): $(LIBC_TEST)/src/common/test.h | $(OBJDIRS)
$(OBJDIR)/%.wasm.o: $(LIBC_TEST)/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIRS):
	mkdir -p $@

clean::
	rm -rf $(OBJDIR)

##### RUN ######################################################################

ENGINE ?= $(WASMTIME) run
ERRS:=$(WASMS:%.core.wasm=%.wasm.err)

# Use the provided Wasm engine to execute each test, emitting its output into
# a `.err` file.
run: build $(ERRS)
	@echo "Tests passed"

$(ERRS): | $(OBJDIRS)

ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
%.wasm.err: %.wasm
	$(ENGINE) --wasm component-model $< >$@
else
%.wasm.err: %.core.wasm
	$(ENGINE) $< >$@
endif

clean::
	rm -rf $(OBJDIR)/*/*.err

##### MISC #####################################################################

# Note: the `clean` target has been built up by all of the previous sections.

debug:
	@echo NAMES $(NAMES)
	@echo TESTS $(TESTS)
	@echo WASMS $(WASMS)
	@echo WASM_OBJS $(WASM_OBJS)
	@echo ERRS $(ERRS)
	@echo DIRS $(DIRS)
	@echo OBJDIRS $(OBJDIRS)

.PHONY: test download build run clean
