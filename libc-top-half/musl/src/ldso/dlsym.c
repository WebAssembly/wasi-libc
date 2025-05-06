#include "dlfcn.h"
#include "dynlink.h"
#include "wasi/api.h"
#include "string.h"

void *dlsym(void *restrict p, const char *restrict s)
{
	__wasi_size_t ret;
	char err_buf[256];
	err_buf[0] = '\0';

	int err = __wasi_dlsym((__wasi_dl_handle_t)p, s, (uint8_t *)err_buf, sizeof(err_buf), &ret);

	if (err != 0)
	{
		if (err_buf[0] != '\0')
		{
			__dl_seterr("%s", err_buf);
		}
		else
		{
			__dl_seterr("dlsym failed with error %s", strerror(err));
		}
		return NULL;
	}

	return (void *)ret;
}
