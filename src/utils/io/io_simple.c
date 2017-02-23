
#include "../misc.h"
#include "../../api.h"
#include "../../intp.h"
#include "../../types/all.h"
#include "io.h"
#include "io_simple.h"

#include <errno.h>
#include <sys/stat.h>

#ifdef PLATFORM_WINDOWS
#include <io.h>
#include <Windows.h>
#define fileno(fp)                  _fileno(fp)
#define read(fd,buffer,count)       read(fd,buffer,count)
#define write(fd,buffer,count)      write(fd,buffer,count)
#else
#include <unistd.h>
#include <fcntl.h>
#endif


void* pylt_io_simple_fileopen(PyLiteInterpreter *I, PyLiteStrObject *fn, int mode) {
    int fd;
    int err = 0;
#ifdef PLATFORM_WINDOWS
    wchar_t cfn[256];

    int ret = uc_ucs4str_to_wcharz(fn->ob_val, fn->ob_size, (wchar_t*)&cfn, 255);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    int openflag = pylt_io_mode_to_posix(mode);
    if (openflag == -1) {
        pl_error(I, pl_static.str.ValueError, "must have exactly one of create/read/write/append mode");
        return NULL;
    }

    fd = _wopen(cfn, openflag);
#else
    char fn_u8[1024];

    int ret = uc_ucs4str_to_utf8z(fn->ob_val, fn->ob_size, (char*)&fn_u8, 1023);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    int openflag = pylt_io_mode_to_posix(mode);
    if (openflag == -1) {
        pl_error(I, pl_static.str.ValueError, "must have exactly one of create/read/write/append mode");
        return NULL;
    }

    fd = open(cfn, openmode);
#endif
    if (fd != -1) {
        PyLiteFileSimple* pfs = pylt_malloc(I, sizeof(PyLiteFileSimple));
        pfs->fno = fd;
        pfs->flag = openflag;

        struct stat stbuf;
        fstat(fd, &stbuf);
        pfs->size = stbuf.st_size;
        return pfs;
    } else {
        switch (errno) {
            case ENOENT:
                pl_error(I, pl_static.str.FileNotFoundError, "[Errno 2] No such file or directory: %r", fn);
                break;
            case EACCES: // open dir, windows
                pl_error(I, pl_static.str.PermissionError, "[Errno 13] Permission denied: %r", fn);
                break;
            case EISDIR: // open dir, linux
                pl_error(I, pl_static.str.IsADirectoryError, "[Errno 21] Is a directory: %r", fn);
                break;
            default:
                pl_error(I, pl_static.str.OSError, "[Errno %d] File Open Error: %r", pylt_obj_int_new(I, errno), fn);
        }
        return NULL;
    }
}

pl_int_t pylt_io_simple_fileclose(PyLiteInterpreter *I, void *pfdata) {
    PyLiteFileSimple *pfs = (PyLiteFileSimple*)pfdata;
    return close(pfs->fno);
}

pl_int_t pylt_io_simple_read(PyLiteInterpreter *I, void *pfdata, uint8_t *buf, pl_uint_t count) {
    PyLiteFileSimple *pfs = (PyLiteFileSimple*)pfdata;
    return read(pfs->fno, buf, count);
}

pl_int_t pylt_io_simple_write(PyLiteInterpreter *I, void *pfdata, uint8_t *buf, pl_uint_t count) {
    PyLiteFileSimple *pfs = (PyLiteFileSimple*)pfdata;
    return write(pfs->fno, buf, count);
}

PyLiteIO PyLiteIOSimple = {
    .open = &pylt_io_simple_fileopen,
    .close = &pylt_io_simple_fileclose,
    .read = &pylt_io_simple_read,
    .write = &pylt_io_simple_write
};
