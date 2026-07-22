#include <threads.h>
#include "syscall.h"
#include <sched.h>

void thrd_yield()
{
	sched_yield();
}
