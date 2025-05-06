#include "dlfcn.h"
#include "dynlink.h"
#include "wasi/api.h"
#include "string.h"

int dlclose(void *p)
{
	char err_buf[256];
	err_buf[0] = '\0';

	int err = __wasi_dlclose((__wasi_dl_handle_t)p, (uint8_t *)err_buf, sizeof(err_buf));

	if (err != 0)
	{
		if (err_buf[0] != '\0')
		{
			__dl_seterr("%s", err_buf);
		}
		else
		{
			__dl_seterr("dlclose failed with error %s", strerror(err));
		}
		return -1;
	}

	return 0;
}
