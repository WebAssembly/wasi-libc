# Smoke test suite specific to wasi-libc
#
# This Makefile is included by the parent Makefile

SMOKE_TESTS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
SMOKE_TESTS := $(wildcard $(SMOKE_TESTS_DIR)/*.c)
SMOKE_OBJDIR := $(OBJDIR)/smoke
SMOKE_WASMS := $(SMOKE_TESTS:$(SMOKE_TESTS_DIR)/%.c=$(SMOKE_OBJDIR)/%.core.wasm)
SMOKE_ERRS := $(SMOKE_WASMS:%.core.wasm=%.wasm.err)

$(SMOKE_OBJDIR):
	mkdir -p $@

$(SMOKE_OBJDIR)/%.core.wasm: $(SMOKE_TESTS_DIR)/%.c | $(SMOKE_OBJDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
$(SMOKE_OBJDIR)/%.wasm: $(SMOKE_OBJDIR)/%.core.wasm
	$(WASM_TOOLS) component new --adapt $(ADAPTER) $< -o $@
endif

ifeq ($(TARGET_TRIPLE), wasm32-wasip2)
$(SMOKE_OBJDIR)/%.wasm.err: $(SMOKE_OBJDIR)/%.wasm
	rm -rf $(SMOKE_TESTS_DIR)/$*.dir
	mkdir -p $(SMOKE_TESTS_DIR)/$*.dir
	$(ENGINE) --dir $(SMOKE_TESTS_DIR)/$*.dir::/ --wasm component-model $< >$@
else
$(SMOKE_OBJDIR)/%.wasm.err: $(SMOKE_OBJDIR)/%.core.wasm
	rm -rf $(SMOKE_TESTS_DIR)/$*.dir
	mkdir -p $(SMOKE_TESTS_DIR)/$*.dir
	$(ENGINE) --dir $(SMOKE_TESTS_DIR)/$*.dir::/ $< >$@
endif

run_smoke: $(SMOKE_ERRS)
	@echo "Smoke tests passed"
