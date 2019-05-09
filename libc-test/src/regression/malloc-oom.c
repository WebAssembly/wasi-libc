// malloc should set errno on oom
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "test.h"

int main(void)
{
#ifdef __wasilibc_unmodified_upstream // Disable this OOM test as it takes too long to run in practice
	if (t_memfill() < 0)
		t_error("memfill failed\n");

	errno = 0;
	if (malloc(50000))
		t_error("malloc was successful\n");
	if (errno != ENOMEM)
		t_error("expected ENOMEM, got %s\n", strerror(errno));
#endif

	return t_status;
}
