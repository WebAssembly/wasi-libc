#include <threads.h>
#include "syscall.h"

void thrd_yield()
{
	sched_yield();
}
