#include <wasi/api.h>

#ifdef __wasip2__
#include <errno.h>
#include <stdlib.h>

static void translate_error(filesystem_error_code_t error) {
    switch (error) {
    case FILESYSTEM_ERROR_CODE_ACCESS:
        errno = EACCES;
        break;
    case FILESYSTEM_ERROR_CODE_WOULD_BLOCK:
        errno = EAGAIN;
        break;
    case FILESYSTEM_ERROR_CODE_ALREADY:
        errno = EALREADY;
        break;
    case FILESYSTEM_ERROR_CODE_BAD_DESCRIPTOR:
        errno = EBADF;
        break;
    case FILESYSTEM_ERROR_CODE_BUSY:
        errno = EBUSY;
        break;
    case FILESYSTEM_ERROR_CODE_DEADLOCK:
        errno = EDEADLK;
        break;
    case FILESYSTEM_ERROR_CODE_QUOTA:
        errno = EDQUOT;
        break;
    case FILESYSTEM_ERROR_CODE_EXIST:
        errno = EEXIST;
        break;
    case FILESYSTEM_ERROR_CODE_FILE_TOO_LARGE:
        errno = EFBIG;
        break;
    case FILESYSTEM_ERROR_CODE_ILLEGAL_BYTE_SEQUENCE:
        errno = EILSEQ;
        break;
    case FILESYSTEM_ERROR_CODE_IN_PROGRESS:
        errno = EINPROGRESS;
        break;
    case FILESYSTEM_ERROR_CODE_INTERRUPTED:
        errno = EINTR;
        break;
    case FILESYSTEM_ERROR_CODE_INVALID:
        errno = EINVAL;
        break;
    case FILESYSTEM_ERROR_CODE_IO:
        errno = EIO;
        break;
    case FILESYSTEM_ERROR_CODE_IS_DIRECTORY:
        errno = EISDIR;
        break;
    case FILESYSTEM_ERROR_CODE_LOOP:
        errno = ELOOP;
        break;
    case FILESYSTEM_ERROR_CODE_TOO_MANY_LINKS:
        errno = EMLINK;
        break;
    case FILESYSTEM_ERROR_CODE_MESSAGE_SIZE:
        errno = EMSGSIZE;
        break;
    case FILESYSTEM_ERROR_CODE_NAME_TOO_LONG:
        errno = ENAMETOOLONG;
        break;
    case FILESYSTEM_ERROR_CODE_NO_DEVICE:
        errno = ENODEV;
        break;
    case FILESYSTEM_ERROR_CODE_NO_ENTRY:
        errno = ENOENT;
        break;
    case FILESYSTEM_ERROR_CODE_NO_LOCK:
        errno = ENOLCK;
        break;
    case FILESYSTEM_ERROR_CODE_INSUFFICIENT_MEMORY:
        errno = ENOMEM;
        break;
    case FILESYSTEM_ERROR_CODE_INSUFFICIENT_SPACE:
        errno = ENOSPC;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_DIRECTORY:
        errno = ENOTDIR;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_EMPTY:
        errno = ENOTEMPTY;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_RECOVERABLE:
        errno = ENOTRECOVERABLE;
        break;
    case FILESYSTEM_ERROR_CODE_UNSUPPORTED:
        errno = ENOTSUP;
        break;
    case FILESYSTEM_ERROR_CODE_NO_TTY:
        errno = ENOTTY;
        break;
    case FILESYSTEM_ERROR_CODE_NO_SUCH_DEVICE:
        errno = ENXIO;
        break;
    case FILESYSTEM_ERROR_CODE_OVERFLOW:
        errno = EOVERFLOW;
        break;
    case FILESYSTEM_ERROR_CODE_NOT_PERMITTED:
        errno = EPERM;
        break;
    case FILESYSTEM_ERROR_CODE_PIPE:
        errno = EPIPE;
        break;
    case FILESYSTEM_ERROR_CODE_READ_ONLY:
        errno = EROFS;
        break;
    case FILESYSTEM_ERROR_CODE_INVALID_SEEK:
        errno = ESPIPE;
        break;
    case FILESYSTEM_ERROR_CODE_TEXT_FILE_BUSY:
        errno = ETXTBSY;
        break;
    case FILESYSTEM_ERROR_CODE_CROSS_DEVICE:
        errno = EXDEV;
        break;
    default:
        abort(); // Unreachable
    }
}
#endif
