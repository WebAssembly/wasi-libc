#include <dlfcn.h>
#include "dynlink.h"
#include "wasi/api.h"

int dlclose(void *p)
{
	int err = __wasi_dl_invalid_handle((__wasi_dl_handle_t)p);
	if (err != 0)
	{
		__dl_seterr("Invalid dl handle");
		return -1;
	}
	return 0;
}
