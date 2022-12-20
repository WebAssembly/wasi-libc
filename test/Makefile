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
# Like the top-level `wasi-libc` Makefile, here we can decide whether to test
# with threads (`posix`) or without (`single`). IMPORTANT: the top-level include
# directory must be built with the same value.
THREAD_MODEL ?= single

##### DOWNLOAD #################################################################

LIBC_TEST_URL ?= https://github.com/bytecodealliance/libc-test
LIBC_TEST = $(DOWNDIR)/libc-test
LIBRT_URL ?= https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-16/libclang_rt.builtins-wasm32-wasi-16.0.tar.gz
LIBRT = $(DOWNDIR)/lib/wasi/libclang_rt.builtins-wasm32.a
WASMTIME_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v3.0.0/wasmtime-v3.0.0-x86_64-linux.tar.xz
WASMTIME = $(DOWNDIR)/$(shell basename $(WASMTIME_URL) .tar.xz)/wasmtime

download: $(LIBC_TEST) $(LIBRT) $(WASMTIME)

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

ifeq ($(THREAD_MODEL), posix)
	TESTS += \
		$(LIBC_TEST)/src/functional/pthread_cond.c \
		$(LIBC_TEST)/src/functional/pthread_tsd.c \
		$(LIBC_TEST)/src/functional/tls_init.c \
		$(LIBC_TEST)/src/functional/tls_local_exec.c
endif

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
WASMS := $(TESTS:$(LIBC_TEST)/src/%.c=$(OBJDIR)/%.wasm)
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
CFLAGS ?= --sysroot=../sysroot
# Always include the `libc-test` infrastructure headers.
CFLAGS += -I$(LIBC_TEST)/src/common

# Configure support for threads.
ifeq ($(THREAD_MODEL), single)
CFLAGS += --target=wasm32-wasi -mthread-model single
endif
ifeq ($(THREAD_MODEL), posix)
# Specify the tls-model until LLVM 15 is released (which should contain
# https://reviews.llvm.org/D130053).
CFLAGS += --target=wasm32-wasi-pthread -mthread-model posix -pthread -ftls-model=local-exec
# NOTE: `--export-memory` is invalid until https://reviews.llvm.org/D135898 is
# available in a released version of `wasm-ld`; this has not happened yet so
# a built-from-latest-source Clang is necessary.
LDFLAGS += -Wl,--import-memory,--export-memory,--max-memory=1048576
endif

# We want to be careful to rebuild the Wasm files if any important variables
# change. This block calculates a hash from key variables and writes a file
# (ENV_HASH = `build/env-<hash>`); any targets dependent on `ENV_HASH` will thus
# be rebuilt whenever `ENV_HASH` is modified, which is whenever `ENV` changes.
# The phony `env` target can be used to debug this.
ENV = "CC=$(CC);CFLAGS=$(CFLAGS);LDFLAGS=$(LDFLAGS);THREAD_MODEL=$(THREAD_MODEL)"
export ENV
ENV_HASH = $(OBJDIR)/env-$(shell echo $(ENV) | md5sum | cut -d ' ' -f 1)
$(ENV_HASH): | $(OBJDIRS)
	rm -f $(OBJDIR)/env-*
	echo $(ENV) > $@
env: $(ENV_HASH)
	@echo ENV = "$$ENV"
	@echo ENV_HASH = $(ENV_HASH)
	cat $^
.PHONY: env

# Compile each selected test using Clang. Note that failures here are likely
# due to a missing `libclang_rt.builtins-wasm32.a` in the Clang lib directory.
# This location is system-dependent, but could be fixed by something like:
#  $ sudo mkdir /usr/lib64/clang/14.0.5/lib/wasi
#  $ sudo cp download/lib/wasi/libclang_rt.builtins-wasm32.a /usr/lib64/clang/14.0.5/lib/wasi/
build: download $(WASMS) $(ENV_HASH)

$(WASMS): | $(OBJDIRS)
$(OBJDIR)/%.wasm: $(OBJDIR)/%.wasm.o $(INFRA_WASM_OBJS) $(ENV_HASH)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter-out $(ENV_HASH), $^) 

$(WASM_OBJS): $(LIBC_TEST)/src/common/test.h | $(OBJDIRS)
$(OBJDIR)/%.wasm.o: $(LIBC_TEST)/src/%.c $(ENV_HASH)
	$(CC) $(CFLAGS) -c -o $@ $< 

$(OBJDIRS):
	mkdir -p $@

clean::
	rm -rf $(OBJDIR)

##### RUN ######################################################################

ENGINE ?= $(WASMTIME) run
ENGINE_FLAGS ?= --wasm-features threads --wasi-modules experimental-wasi-threads
ERRS:=$(WASMS:%.wasm=%.wasm.err)

# Use the provided Wasm engine to execute each test, emitting its output into
# a `.err` file.
run: build $(ERRS)
	@echo "Tests passed"

$(ERRS): | $(OBJDIRS)

%.wasm.err: %.wasm
	$(ENGINE) $(ENGINE_FLAGS) $<

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
