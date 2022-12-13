#include "stdio_impl.h"

static FILE *volatile dummy_file = 0;
weak_alias(dummy_file, __stdin_used);
weak_alias(dummy_file, __stdout_used);
weak_alias(dummy_file, __stderr_used);

static void close_file(FILE *f)
{
	if (!f) return;
#ifdef __wasilibc_unmodified_upstream
	FFINALLOCK(f);
#else
	FLOCK(f);
#endif
	if (f->wpos != f->wbase) f->write(f, 0, 0);
	if (f->rpos != f->rend) f->seek(f, f->rpos-f->rend, SEEK_CUR);
#ifndef __wasilibc_unmodified_upstream
	FUNLOCK(f);
#endif
}

void __stdio_exit(void)
{
	FILE *f;
	for (f=*__ofl_lock(); f; f=f->next) close_file(f);
	close_file(__stdin_used);
	close_file(__stdout_used);
	close_file(__stderr_used);
}

weak_alias(__stdio_exit, __stdio_exit_needed);
