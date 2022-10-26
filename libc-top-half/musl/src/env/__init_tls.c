#define SYSCALL_NO_TLS 1
#ifdef __wasilibc_unmodified_upstream
#include <elf.h>
#endif
#include <limits.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/mman.h>
#endif
#include <string.h>
#include <stddef.h>
#include "pthread_impl.h"
#include "libc.h"
#include "atomic.h"
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

volatile int __thread_list_lock;

int __init_tp(void *p)
{
	pthread_t td = p;
	td->self = td;
	int r = __set_thread_area(TP_ADJ(p));
	if (r < 0) return -1;
	if (!r) libc.can_do_threads = 1;
	td->detach_state = DT_JOINABLE;
#ifdef __wasilibc_unmodified_upstream
	td->tid = __syscall(SYS_set_tid_address, &__thread_list_lock);
#endif
	td->locale = &libc.global_locale;
	td->robust_list.head = &td->robust_list.head;
	td->sysinfo = __sysinfo;
	td->next = td->prev = td;
	return 0;
}

static struct builtin_tls {
	char c;
	struct pthread pt;
	void *space[16];
} builtin_tls[1];
#define MIN_TLS_ALIGN offsetof(struct builtin_tls, pt)

static struct tls_module main_tls;

void *__copy_tls(unsigned char *mem)
{
	pthread_t td;
	struct tls_module *p;
	size_t i;
	uintptr_t *dtv;

#ifdef TLS_ABOVE_TP
	dtv = (uintptr_t*)(mem + libc.tls_size) - (libc.tls_cnt + 1);

	mem += -((uintptr_t)mem + sizeof(struct pthread)) & (libc.tls_align-1);
	td = (pthread_t)mem;
	mem += sizeof(struct pthread);

	for (i=1, p=libc.tls_head; p; i++, p=p->next) {
		dtv[i] = (uintptr_t)(mem + p->offset) + DTP_OFFSET;
		memcpy(mem + p->offset, p->image, p->len);
	}
#else
	dtv = (uintptr_t *)mem;

	mem += libc.tls_size - sizeof(struct pthread);
	mem -= (uintptr_t)mem & (libc.tls_align-1);
	td = (pthread_t)mem;

#ifdef __wasilibc_unmodified_upstream
	for (i=1, p=libc.tls_head; p; i++, p=p->next) {
		dtv[i] = (uintptr_t)(mem - p->offset) + DTP_OFFSET;
		memcpy(mem - p->offset, p->image, p->len);
	}
#endif
#endif
	dtv[0] = libc.tls_cnt;
	td->dtv = dtv;
	return td;
}

#ifdef __wasilibc_unmodified_upstream
#if ULONG_MAX == 0xffffffff
typedef Elf32_Phdr Phdr;
#else
typedef Elf64_Phdr Phdr;
#endif

extern weak hidden const size_t _DYNAMIC[];
#endif

#ifdef __wasilibc_unmodified_upstream
static void static_init_tls(size_t *aux)
#else
void __init_tls(size_t *aux)
#endif
{
	void *mem;
#ifdef __wasilibc_unmodified_upstream
	unsigned char *p;
	size_t n;
	Phdr *phdr, *tls_phdr=0;
	size_t base = 0;

	for (p=(void *)aux[AT_PHDR],n=aux[AT_PHNUM]; n; n--,p+=aux[AT_PHENT]) {
		phdr = (void *)p;
		if (phdr->p_type == PT_PHDR)
			base = aux[AT_PHDR] - phdr->p_vaddr;
		if (phdr->p_type == PT_DYNAMIC && _DYNAMIC)
			base = (size_t)_DYNAMIC - phdr->p_vaddr;
		if (phdr->p_type == PT_TLS)
			tls_phdr = phdr;
		if (phdr->p_type == PT_GNU_STACK &&
		    phdr->p_memsz > __default_stacksize)
			__default_stacksize =
				phdr->p_memsz < DEFAULT_STACK_MAX ?
				phdr->p_memsz : DEFAULT_STACK_MAX;
	}

	if (tls_phdr) {
		main_tls.image = (void *)(base + tls_phdr->p_vaddr);
		main_tls.len = tls_phdr->p_filesz;
		main_tls.size = tls_phdr->p_memsz;
		main_tls.align = tls_phdr->p_align;
		libc.tls_cnt = 1;
		libc.tls_head = &main_tls;
	}

	main_tls.size += (-main_tls.size - (uintptr_t)main_tls.image)
		& (main_tls.align-1);
#ifdef TLS_ABOVE_TP
	main_tls.offset = GAP_ABOVE_TP;
	main_tls.offset += (-GAP_ABOVE_TP + (uintptr_t)main_tls.image)
		& (main_tls.align-1);
#else
	main_tls.offset = main_tls.size;
#endif
	if (main_tls.align < MIN_TLS_ALIGN) main_tls.align = MIN_TLS_ALIGN;
#else // __wasilibc_unmodified_upstream
	main_tls.size = __builtin_wasm_tls_size();
	main_tls.offset = main_tls.size;
	main_tls.align = __builtin_wasm_tls_align();
	if (main_tls.size > 0)
		libc.tls_cnt = 1;
#endif // __wasilibc_unmodified_upstream
	libc.tls_align = main_tls.align;
	libc.tls_size = 2*sizeof(void *) + sizeof(struct pthread)
#ifdef TLS_ABOVE_TP
		+ main_tls.offset
#endif
		+ main_tls.size + main_tls.align
		+ MIN_TLS_ALIGN-1 & -MIN_TLS_ALIGN;

#ifdef __wasilibc_unmodified_upstream // FIXME
	if (libc.tls_size > sizeof builtin_tls) {
#ifndef SYS_mmap2
#define SYS_mmap2 SYS_mmap
#endif
		mem = (void *)__syscall(
			SYS_mmap2,
			0, libc.tls_size, PROT_READ|PROT_WRITE,
			MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
		/* -4095...-1 cast to void * will crash on dereference anyway,
		 * so don't bloat the init code checking for error codes and
		 * explicitly calling a_crash(). */
	} else {
		mem = builtin_tls;
	}
#else // __wasilibc_unmodified_upstream
	mem = builtin_tls;
#endif // __wasilibc_unmodified_upstream

	/* Failure to initialize thread pointer is always fatal. */
	if (__init_tp(__copy_tls(mem)) < 0)
		a_crash();
}

#ifdef __wasilibc_unmodified_upstream
weak_alias(static_init_tls, __init_tls);
#endif
