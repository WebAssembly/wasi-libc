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

# Decide which target to build for and which libc to use.
TARGET_TRIPLE ?= wasm32-wasi

# Setup various paths used by the tests.
OBJDIR ?= build/$(TARGET_TRIPLE)
DOWNDIR ?= build/download
SRCDIR ?= src
RUNDIR ?= run/$(TARGET_TRIPLE)

# We also need to know the location the wasi-libc sysroot we're building
# against.
SYSROOT_DIR ?= ../sysroot
SYSROOT := $(SYSROOT_DIR)/lib/$(TARGET_TRIPLE)
$(SYSROOT):
	@echo "No sysroot for $(TARGET_TRIPLE) available at $(SYSROOT_DIR); to build it, e.g.:"
	@echo "  cd $(dir $(SYSROOT_DIR))"
	@echo "  make TARGET_TRIPLE=$(TARGET_TRIPLE)"
	@exit 1


##### DOWNLOAD #################################################################

LIBC_TEST_URL ?= https://github.com/bytecodealliance/libc-test
LIBC_TEST = $(DOWNDIR)/libc-test
LIBRT_URL ?= https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-24/libclang_rt.builtins-wasm32-wasi-24.0.tar.gz
LIBRT = $(DOWNDIR)/libclang_rt.builtins-wasm32.a
UNAME_D := $(shell uname -m)
ifeq ($(UNAME_D),aarch64)
	WASMTIME_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v26.0.1/wasmtime-v26.0.1-aarch64-linux.tar.xz 
else
	WASMTIME_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v26.0.1/wasmtime-v26.0.1-x86_64-linux.tar.xz
endif
WASMTIME = $(abspath $(DOWNDIR)/$(shell basename $(WASMTIME_URL) .tar.xz)/wasmtime)
ifeq ($(UNAME_D),aarch64)
	WASM_TOOLS_URL ?= https://github.com/bytecodealliance/wasm-tools/releases/download/v1.220.0/wasm-tools-1.220.0-aarch64-linux.tar.gz 
else
	WASM_TOOLS_URL ?= https://github.com/bytecodealliance/wasm-tools/releases/download/v1.220.0/wasm-tools-1.220.0-x86_64-linux.tar.gz
endif
WASM_TOOLS = $(DOWNDIR)/$(shell basename $(WASM_TOOLS_URL) .tar.gz)/wasm-tools
ADAPTER_URL ?= https://github.com/bytecodealliance/wasmtime/releases/download/v26.0.1/wasi_snapshot_preview1.command.wasm
ADAPTER = $(DOWNDIR)/wasi_snapshot_preview1.command.wasm

$(DOWNDIR):
	@mkdir -p $@

$(LIBC_TEST): | $(DOWNDIR)
	git clone --depth 1 $(LIBC_TEST_URL) $@

# TODO: add install target to copy builtins library directly into a Clang
# installation.
$(LIBRT): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(LIBRT_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(LIBRT_URL)) --strip-components=1 --directory=$(DOWNDIR)/

$(WASMTIME): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(WASMTIME_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(WASMTIME_URL)) --directory=$(DOWNDIR)/

$(WASM_TOOLS): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(WASM_TOOLS_URL)
	tar --extract --file=$(DOWNDIR)/$(shell basename $(WASM_TOOLS_URL)) --directory=$(DOWNDIR)/

$(ADAPTER): | $(DOWNDIR)
	wget --no-clobber --directory-prefix=$(DOWNDIR) $(ADAPTER_URL)

# Target to download all necessary dependencies.
TO_DOWNLOAD = $(LIBC_TEST) $(LIBRT) $(WASMTIME)
ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
TO_DOWNLOAD += $(ADAPTER) $(WASM_TOOLS)
endif
DOWNLOADED := $(DOWNDIR)/downloaded.stamp
$(DOWNLOADED): $(TO_DOWNLOAD)
	touch $@
download: $(DOWNLOADED)

clean::
	rm -rf $(DOWNDIR)

##### INFRA ####################################################################

INFRA_OBJDIR := $(OBJDIR)/common
$(INFRA_OBJDIR):
	@mkdir -p $@

# Build the common test infrastructure. Part of the problem including more tests
# is that the `libc-test` infrastructure code is not all Wasm-compilable. As we
# include more tests above, this list will also likely need to grow.
INFRA_FILES = $(LIBC_TEST)/src/common/path.c \
	$(LIBC_TEST)/src/common/print.c \
	$(LIBC_TEST)/src/common/rand.c \
	$(LIBC_TEST)/src/common/utf8.c
$(INFRA_FILES): $(DOWNLOADED)
INFRA_WASM_OBJS := $(patsubst $(LIBC_TEST)/src/common/%.c,$(OBJDIR)/common/%.wasm.o,$(INFRA_FILES))
$(OBJDIR)/common/%.wasm.o: $(LIBC_TEST)/src/common/%.c | $(INFRA_OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Also, include the `libc-test` infrastructure headers.
INFRA_HEADERS_DIR := $(LIBC_TEST)/src/common
INFRA_HEADERS := $(shell find $(INFRA_HEADERS_DIR) -name '*.h')
$(INFRA_HEADERS): $(DOWNLOADED)

##### BUILD ####################################################################

# Create various lists containing the various artifacts to be built: mainly,
# $(WASM_OBJS) are compiled in the $(OBJDIRS) and then linked together to form
# the $(WASMS) tests.
ALL_TESTS := $(shell find $(SRCDIR) -name '*.c')
TESTS := $(shell TARGET_TRIPLE=$(TARGET_TRIPLE) scripts/filter.py $(ALL_TESTS))
WASM_OBJS := $(TESTS:$(SRCDIR)/%.c=$(OBJDIR)/%.wasm.o)
WASM_OBJS += $(INFRA_WASM_OBJS)
ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
WASMS := $(TESTS:$(SRCDIR)/%.c=$(OBJDIR)/%.component.wasm)
else
WASMS := $(TESTS:$(SRCDIR)/%.c=$(OBJDIR)/%.core.wasm)
endif


# Setup the compiler. We allow $(CC) to be set from the command line; ?= doesn't
# work for CC because make has a default value for it.
ifeq ($(origin CC), default)
CC := clang
endif
LDFLAGS ?=
CFLAGS ?= --target=$(TARGET_TRIPLE) --sysroot=$(SYSROOT_DIR)
# Always include the `libc-test` infrastructure headers.
CFLAGS += -I$(INFRA_HEADERS_DIR)
ifneq ($(findstring -threads,$(TARGET_TRIPLE)),)
CFLAGS += -pthread
endif

# Build up all the `*.wasm.o` object files; these are the same regardless of
# whether we're building core modules or components.
$(WASM_OBJS): $(INFRA_HEADERS)
$(OBJDIR)/%.wasm.o: $(SRCDIR)/%.c $(DOWNLOADED) $(SYSROOT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(shell scripts/add-flags.py CFLAGS $<) -c $< -o $@

# Build up all the `*.wasm` files.
obj_to_c = $(patsubst $(OBJDIR)/%.wasm.o,$(SRCDIR)/%.c,$1)
$(OBJDIR)/%.core.wasm: $(OBJDIR)/%.wasm.o $(INFRA_WASM_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) $(shell scripts/add-flags.py LDFLAGS $(call obj_to_c,$<)) $^ -o $@

# For wasip2, we include an additional step to wrap up the core module into
# a component.
$(OBJDIR)/%.component.wasm: $(OBJDIR)/%.core.wasm
	$(WASM_TOOLS) component new --adapt $(ADAPTER) $< -o $@

# Compile each selected test using Clang. Note that failures here are likely
# due to a missing `libclang_rt.builtins-wasm32.a` in the Clang lib directory.
# This location is system-dependent, but could be fixed by something like:
#  $ sudo mkdir /usr/lib64/clang/14.0.5/lib/wasi
#  $ sudo cp download/libclang_rt.builtins-wasm32.a /usr/lib64/clang/14.0.5/lib/wasi/
build: $(DOWNLOADED) $(WASMS)

clean::
	rm -rf $(OBJDIR)

##### GENERATE #################################################################

# Not all of the downloaded `libc-test` tests can be built and run with
# `wasi-libc`. Thus, we only include the subset that can be in `src/libc-test`
# as stub files that `#include` the original test files. When we want to add
# more tests, though, the `generate-stubs` target will generate stubs for the
# missing tests which we can delete or alter as needed.

STUBDIR := $(SRCDIR)/libc-test
generate-stubs:
	FROM_DIR=$(LIBC_TEST) TO_DIR=$(STUBDIR) scripts/generate-stubs.sh

##### RUN ######################################################################

ENGINE ?= $(WASMTIME) run
ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
ENGINE += --wasm component-model
OBJPAT := $(OBJDIR)/%.component.wasm
else
OBJPAT := $(OBJDIR)/%.core.wasm
endif

# Each Wasm test is run every time, generating a folder containing a `cmd.sh`
# script and an `output.log` file (see `scripts/run-test.sh` for details). The
# `success` file is never generated, which means the test will rerun every time.
# To ignore a test temporarily, `touch .../success:`.
RUNTESTS:=$(WASMS:$(OBJPAT)=$(RUNDIR)/%/success)
wasm_to_c = $(patsubst $(OBJPAT),$(SRCDIR)/%.c,$1)
$(RUNDIR)/%/success: $(OBJPAT)
	@mkdir -p $(@D)
	@DIR="$(abspath $(@D))" \
	  WASM="$(abspath $<)" \
	  ENGINE="$(ENGINE) $(shell scripts/add-flags.py RUN $(call wasm_to_c,$<))" \
	    scripts/run-test.sh

# Use the provided Wasm engine to execute each test, emitting its output into
# a `.err` file.
run: build $(RUNTESTS)
	@if scripts/failed-tests.sh $(RUNDIR); then \
		echo "Tests passed"; \
	else \
		echo "Tests failed:"; \
		VERBOSE=1 scripts/failed-tests.sh $(RUNDIR); \
	fi

clean::
	rm -rf $(RUNDIR)

##### MISC #####################################################################

# Note: the `clean` target has been built up by all of the previous sections.

.PHONY: test download build run generate-stubs clean
