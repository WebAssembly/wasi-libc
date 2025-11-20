#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <errno.h>
#include <stdlib.h>

static void translate_error(filesystem_error_code_t error) {
    switch (error) {
    case FILESYSTEM_ERROR_CODE_ACCESS:
        errno = __WASI_ERRNO_ACCES;
        break;
    case FILESYSTEM_ERROR_CODE_WOULD_BLOCK:
        errno = __WASI_ERRNO_AGAIN;
        break;
    case FILESYSTEM_ERROR_CODE_ALREADY:
        errno = __WASI_ERRNO_ALREADY;
        break;
    case FILESYSTEM_ERROR_CODE_BAD_DESCRIPTOR:
        errno = __WASI_ERRNO_BADF;
        break;
    case FILESYSTEM_ERROR_CODE_BUSY:
        errno = __WASI_ERRNO_BUSY;
        break;
    case FILESYSTEM_ERROR_CODE_DEADLOCK:
        errno = __WASI_ERRNO_DEADLK;
        break;
    case FILESYSTEM_ERROR_CODE_QUOTA:
        errno = __WASI_ERRNO_DQUOT;
        break;
    case FILESYSTEM_ERROR_CODE_EXIST:
        errno = __WASI_ERRNO_EXIST;
        break;
    case FILESYSTEM_ERROR_CODE_FILE_TOO_LARGE:
        errno = __WASI_ERRNO_FBIG;
        break;
    case FILESYSTEM_ERROR_CODE_ILLEGAL_BYTE_SEQUENCE:
        errno = __WASI_ERRNO_ILSEQ;
        break;
    case FILESYSTEM_ERROR_CODE_IN_PROGRESS:
        errno = __WASI_ERRNO_INPROGRESS;
        break;
    case FILESYSTEM_ERROR_CODE_INTERRUPTED:
        errno = __WASI_ERRNO_INTR;
        break;
    case FILESYSTEM_ERROR_CODE_INVALID:
        errno = __WASI_ERRNO_INVAL;
        break;
    case FILESYSTEM_ERROR_CODE_IO:
        errno = __WASI_ERRNO_IO;
        break;
    case FILESYSTEM_ERROR_CODE_IS_DIRECTORY:
        errno = __WASI_ERRNO_ISDIR;
        break;
    case FILESYSTEM_ERROR_CODE_LOOP:
        errno = __WASI_ERRNO_LOOP;
        break;
    case FILESYSTEM_ERROR_CODE_TOO_MANY_LINKS:
        errno = __WASI_ERRNO_MLINK;
        break;
    case FILESYSTEM_ERROR_CODE_MESSAGE_SIZE:
        errno = __WASI_ERRNO_MSGSIZE;
        break;
    case FILESYSTEM_ERROR_CODE_NAME_TOO_LONG:
        errno = __WASI_ERRNO_NAMETOOLONG;
        break;
    case FILESYSTEM_ERROR_CODE_NO_DEVICE:
        errno = __WASI_ERRNO_NODEV;
        break;
    case FILESYSTEM_ERROR_CODE_NO_ENTRY:
        errno = __WASI_ERRNO_NOENT;
        break;
    case FILESYSTEM_ERROR_CODE_NO_LOCK:
        errno = __WASI_ERRNO_NOLCK;
        break;
    case FILESYSTEM_ERROR_CODE_INSUFFICIENT_MEMORY:
        errno = __WASI_ERRNO_NOMEM;
        break;
    case FILESYSTEM_ERROR_CODE_INSUFFICIENT_SPACE:
        errno = __WASI_ERRNO_NOSPC;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_DIRECTORY:
        errno = __WASI_ERRNO_NOTDIR;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_EMPTY:
        errno = __WASI_ERRNO_NOTEMPTY;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_RECOVERABLE:
        errno = __WASI_ERRNO_NOTRECOVERABLE;
        break;
    case FILESYSTEM_ERROR_CODE_UNSUPPORTED:
        errno = __WASI_ERRNO_NOTSUP;
        break;
    case FILESYSTEM_ERROR_CODE_NO_TTY:
        errno = __WASI_ERRNO_NOTTY;
        break;
    case FILESYSTEM_ERROR_CODE_NO_SUCH_DEVICE:
        errno = __WASI_ERRNO_NXIO;
        break;
    case FILESYSTEM_ERROR_CODE_OVERFLOW:
        errno = __WASI_ERRNO_OVERFLOW;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_PERMITTED:
        errno = __WASI_ERRNO_PERM;
        break;
    case FILESYSTEM_ERROR_CODE_PIPE:
        errno = __WASI_ERRNO_PIPE;
        break;
    case FILESYSTEM_ERROR_CODE_READ_ONLY:
        errno = __WASI_ERRNO_ROFS;
        break;
    case FILESYSTEM_ERROR_CODE_INVALID_SEEK:
        errno = __WASI_ERRNO_SPIPE;
        break;
    case FILESYSTEM_ERROR_CODE_TEXT_FILE_BUSY:
        errno = __WASI_ERRNO_TXTBSY;
        break;
    case FILESYSTEM_ERROR_CODE_CROSS_DEVICE:
        errno = __WASI_ERRNO_XDEV;
        break;
    default:
        abort(); // Unreachable
    }
}
#endif
