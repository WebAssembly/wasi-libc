#include "dlfcn.h"
#include "dynlink.h"
#include "wasi/api.h"
#include "string.h"
#include "stdlib.h"

void *dlopen(const char *file, int mode)
{
	__wasi_dl_handle_t ret = 0;
	char err_buf[256];
	err_buf[0] = '\0';

	char *ld_library_path = getenv("LD_LIBRARY_PATH");

	int err = __wasi_dlopen(file, mode, (uint8_t *)err_buf, sizeof(err_buf), ld_library_path, &ret);

	if (err != 0)
	{
		if (err_buf[0] != '\0')
		{
			__dl_seterr("%s", err_buf);
		}
		else
		{
			__dl_seterr("dlopen failed with error %s", strerror(err));
		}
		return NULL;
	}

	return (void *)ret;
}
