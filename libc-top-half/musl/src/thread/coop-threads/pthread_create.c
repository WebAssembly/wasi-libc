#define _GNU_SOURCE
#include "libc.h"
#include "lock.h"
#include "pthread_impl.h"
#include "stdio_impl.h"
#include <stdatomic.h>
#include <stddef.h>
#include <string.h>

#include <assert.h>
#include <stdalign.h>
#include <wasi/api.h>

// These are implemented elsewhere
static void dummy_0() {}
weak_alias(dummy_0, __pthread_tsd_run_dtors);
weak_alias(dummy_0, __do_orphaned_stdio_locks);

static void *map_base_deferred_free;

static void process_map_base_deferred_free() {
  free(map_base_deferred_free);
  map_base_deferred_free = NULL;
}

// Defined in `__wasi_coop_thread_start.s` and not actually callable from C,
// this is the component-model entrypoint.
extern void __wasi_coop_thread_start(void *context);

// Synthesized by wasm-ld this is called at the start of each new thread to
// initialize its own TLS block.
extern void __wasm_init_tls(void *ptr);

// There is currently no thread.exit intrinsic, so pthread_exit
// cannot terminate a thread early. As such, we do not expose it to users.
// TODO(wasip3) revisit this if we add a thread.exit intrinsic
static void __pthread_exit(void *result) {
  pthread_t self = __pthread_self();

  self->canceldisable = 1;
  self->cancelasync = 0;
  self->result = result;

  while (self->cancelbuf) {
    void (*f)(void *) = self->cancelbuf->__f;
    void *x = self->cancelbuf->__x;
    self->cancelbuf = self->cancelbuf->__next;
    f(x);
  }

  __pthread_tsd_run_dtors();

  if (self->detach_state == DT_JOINABLE) {
    self->detach_state = DT_EXITING;
  }
  int state = self->detach_state;

  /* If this is the only thread in the list, don't proceed with
   * termination of the thread, just exit immediately */
  if (self->next == self) {
    self->detach_state = state;
    exit(0);
  }

  // At this point we are committed to thread termination.

  /* After the kernel thread exits, its tid may be reused. Clear it
   * to prevent inadvertent use and inform functions that would use
   * it that it's no longer available. */
  self->tid = 0;

  __do_orphaned_stdio_locks();

  // This is the point where we would handle robust mutexes if we supported them

  // Unlink the thread from the thread list.
  if (!--libc.threads_minus_1)
    libc.need_locks = -1;
  self->next->prev = self->prev;
  self->prev->next = self->next;
  self->prev = self->next = self;

  if (state == DT_DETACHED && self->map_base) {
    /* As we use malloc/free which is considerably more complex
     * than mmap/munmap to call and can even require a valid
     * thread context, it's difficult to implement __unmapself.
     *
     * Here we take an alternative approach which simply defers
     * the deallocation. An obvious downside of this approach is
     * that it keeps the stack longer. (possibly forever.)
     * To avoid wasting too much memory, we only defer a single
     * item at most. */
    process_map_base_deferred_free();
    map_base_deferred_free = self->map_base;
    // Here is where we would call the thread.exit intrinsic
    return;
  }

  self->detach_state = DT_EXITED;

  // There may be a thread waiting for this thread to exit, in which case
  // we wake it up. Note that this specifically doesn't yield to the thread,
  // however, because that would leave this thread able to get reaped and the
  // stack would get deallocated. That means this thread, upon resuming, would
  // UAF its own stack. By not yielding here we're guaranteed that the stack is
  // valid until this function returns.
  __wake(&self->joiner_futex, 1, 0);
}

void __do_cleanup_push(struct __ptcb *cb) {
  struct pthread *self = __pthread_self();
  cb->__next = self->cancelbuf;
  self->cancelbuf = cb;
}

void __do_cleanup_pop(struct __ptcb *cb) {
  __pthread_self()->cancelbuf = cb->__next;
}

struct start_args {
  // Note that this `stack` member must be first as its offset is referenced
  // from `__wasi_coop_thread_start` within the `__wasi_coop_thread_start.s`
  // file.
  //
  // Otherwise though this is the initial stack pointer of the thread-to-be.
  void *stack;

  // The TLS base pointer for this new thread. Not yet initialized, but
  // allocated and has appropriate alignment.
  void *tls_base;

  // A pointer to where this thread's `struct pthread` block is located. This is
  // within `tls_base` and is used during initialization.
  pthread_t self;

  // The arguments used to execute this thread.
  void *(*start_func)(void *);
  void *start_arg;
};

hidden void __wasi_coop_thread_start_C(struct start_args *args) {
  // First thing to do on this new thread is initialize TLS. At the start of a
  // new thread our tls base is 0, so it needs to be set to `args->tls_base`.
  // TLS initialization happens through the `wasm-ld` provided symbol of
  // `__wasm_init_tls` which will `memory.init` this pointer and additionally
  // store the pointer into our tls base slot.
  //
  // Note that this thread's own `struct pthread`, however, lives within the TLS
  // block and is already initialized. We don't want the default initialization
  // here as well. The contents of our pthread block are thus saved/restored
  // around the `__wasm_init_tls` call.
  assert(((uintptr_t)args->tls_base) % __builtin_wasm_tls_align() == 0);
  struct pthread self_copy = *args->self;
  __wasm_init_tls(args->tls_base);
  *__pthread_self() = self_copy;

  // Our tid should be configured in `pthread_create`, but double check it.
  assert(__pthread_self()->tid == wasip3_thread_index());

  __pthread_exit(args->start_func(args->start_arg));
}

/*
 * As we allocate stack with malloc() instead of mmap/mprotect,
 * there is no point to round it up to PAGE_SIZE.
 * Instead, round up to a sane alignment.
 * Note: PAGE_SIZE is rather big on WASM. (65536)
 */
#define ROUND(x) (((x) + 16 - 1) & -16)

/* pthread_key_create.c overrides this */
static volatile size_t dummy = 0;
weak_alias(dummy, __pthread_tsd_size);
static void *dummy_tsd[1] = {0};
weak_alias(dummy_tsd, __pthread_tsd_main);

static FILE *volatile dummy_file = 0;
weak_alias(dummy_file, __stdin_used);
weak_alias(dummy_file, __stdout_used);
weak_alias(dummy_file, __stderr_used);

int __pthread_create(pthread_t *restrict res,
                     const pthread_attr_t *restrict attrp,
                     void *(*entry)(void *), void *restrict arg) {
  int ret, c11 = (attrp == __ATTRP_C11_THREAD);
  size_t size, guard;
  struct pthread *self, *new;
  unsigned char *map = 0, *stack = 0, *tsd = 0, *stack_limit;

  pthread_attr_t attr = {0};

  size_t tls_size = __builtin_wasm_tls_size();
  size_t tls_align = __builtin_wasm_tls_align();
  void *tls_base = __builtin_wasm_tls_base();
  void *new_tls_base;
  size_t tls_offset;
  /* We'll need to allocate space for a correctly-aligned TLS block,
     so adjust the size accordingly. */
  tls_size += tls_align;

  self = __pthread_self();

  if (!libc.threaded) {
    self->tsd = (void **)__pthread_tsd_main;
    libc.threaded = 1;
  }
  if (attrp && !c11)
    attr = *attrp;

  if (!attrp || c11) {
    attr._a_stacksize = __default_stacksize;
    attr._a_guardsize = __default_guardsize;
  }

  if (attr._a_stackaddr) {
    size_t need = tls_size + __pthread_tsd_size;
    size = attr._a_stacksize;
    stack = (void *)(attr._a_stackaddr & -16);
    stack_limit = (void *)(attr._a_stackaddr - size);
    /* Use application-provided stack for TLS only when
     * it does not take more than ~12% or 2k of the
     * application's stack space. */
    if (need < size / 8 && need < 2048) {
      tsd = stack - __pthread_tsd_size;
      stack = tsd - tls_size;
      memset(stack, 0, need);
    } else {
      size = ROUND(need);
    }
    guard = 0;
  } else {
    guard = ROUND(attr._a_guardsize);
    size = guard + ROUND(attr._a_stacksize + tls_size + __pthread_tsd_size);
  }

  if (!tsd) {
    /* Process the deferred free request if any before
     * allocationg a new one. Hopefully it enables a reuse of the memory.
     *
     * Note: We can't perform a simple "handoff" becasue allocation
     * sizes might be different. (eg. the stack size might differ) */
    __tl_lock();
    process_map_base_deferred_free();
    __tl_unlock();
    map = malloc(size);
    if (!map)
      goto fail;
    tsd = map + size - __pthread_tsd_size;
    memset(tsd, 0, __pthread_tsd_size);
    if (!stack) {
      stack = tsd - tls_size;
      stack_limit = map + guard;
    }
  }

  void *unaligned_tls_base = tsd - tls_size;
  new_tls_base = (void *)(((uintptr_t)unaligned_tls_base) & -tls_align);

  /* Compute pthread struct offset from old TLS base, apply to new TLS base */
  tls_offset = (uintptr_t)self - (uintptr_t)tls_base;
  new = (void *)((uintptr_t)new_tls_base + tls_offset);

  // Ensure the pthread structure is at least fully initialized.
  memset(new, 0, sizeof(*new));

  new->map_base = map;
  new->map_size = size;
  new->stack = stack;
  new->stack_size = stack - stack_limit;
  new->guard_size = guard;
  new->tsd = (void *)tsd;
  new->locale = &libc.global_locale;
  new->self = new;
  if (attr._a_detach) {
    new->detach_state = DT_DETACHED;
  } else {
    new->detach_state = DT_JOINABLE;
  }
  new->joiner_futex = 0;
  new->robust_list.head = &new->robust_list.head;
  new->canary = self->canary;
  new->sysinfo = self->sysinfo;

  // Setup argument structure for the new thread on its stack.

  /* Align the stack to struct start_args */
  stack -= sizeof(struct start_args);
  stack -= (uintptr_t)stack % alignof(struct start_args);
  struct start_args *args = (void *)stack;

  /* Align the stack to 16 and store it */
  new->stack = (void *)((uintptr_t)stack & -16);
  /* Correct the stack size */
  new->stack_size = stack - stack_limit;

  args->stack = new->stack; /* just for convenience of asm trampoline */
  args->start_func = entry;
  args->start_arg = arg;
  args->tls_base = (void *)new_tls_base;
  args->self = new;

  if (!libc.threads_minus_1++)
    libc.need_locks = 1;
  /* Instead of `__clone`, WASIP3 uses a host API to instantiate a new version
   * of the current module and start executing the entry function.  */
  ret = wasip3_thread_new_indirect(__wasi_coop_thread_start, (void *)args);

  new->tid = ret;

  // Link the new thread into the thread list
  if (ret >= 0) {
    new->next = self->next;
    new->prev = self;
    new->next->prev = new;
    new->prev->next = new;
  } else {
    if (!--libc.threads_minus_1)
      libc.need_locks = 0;
  }

  if (ret < 0) {
    free(map);
    return -ret;
  }

  // Eagerly run the newly-created thread
  // TODO(wasip3): make this configurable, perhaps through scheduler options?
  wasip3_thread_yield_then_resume(new->tid);

  *res = new;
  return 0;
fail:
  return EAGAIN;
}

// TODO(wasip3) add an alias for pthread_exit when we want to expose it to users
weak_alias(__pthread_create, pthread_create);
