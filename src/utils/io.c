
#include "io.h"
#include "../api.h"
#include "../types/all.h"

#include <io.h>
#include <errno.h>
#include <sys/stat.h>

int check_mode(PyLiteStrObject *mode, int *perr) {
    int err = 0;
    int flags = 0;
    char base_mode = 0;
    pl_bool_t _readable = false;
    pl_bool_t plus = false;

    for (pl_uint_t i = 0; i < mode->ob_size; ++i) {
        switch (mode->ob_val[i]) {
            case 'x': case 'r': case 'w': case 'a':
                if (base_mode) {
                    err = 1;
                    goto _err;
                }
                base_mode = (char)mode->ob_val[i];
                break;
            case '+':
                if (plus) {
                    err = 2;
                    goto _err;
                }
                plus = true;
                break;
            default:
                err = 3;
                goto _err;
        }

        switch (mode->ob_val[i]) {
            case 'x': flags = O_EXCL | O_CREAT; break;
            case 'r': flags = 0; _readable = true; break;
            case 'w': flags = O_CREAT | O_TRUNC; break;
            case 'a': flags = O_APPEND | O_CREAT; break;
            case '+': break;
        }
    }
    
    if (plus) flags |= O_RDWR;
    else if (_readable) flags |= O_RDONLY;
    else flags |= O_WRONLY;

    flags |= O_BINARY;

    return flags;
_err:
    *perr = err;
    return 0;
}


int file_open(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
    int err = 0;
#ifdef PLATFORM_WINDOWS
    wchar_t cfn[256];
    if (fn->ob_size > 255) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return -1;
    }

    if (!ucs4str_to_wchar(fn->ob_val, fn->ob_size, (wchar_t*)&cfn, false)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return -1;
    }

    int flags = check_mode(mode, &err);
    if (err == 0) return _wopen(cfn, flags);
    else {
        pl_error(I, pl_static.str.ValueError, "invalid mode: %r", mode);
        return -1;
    }
#else
    char cfn[1536];
    if (fn->ob_size > 255) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return -1;
    }

    if (!ucs4str_to_utf8(fn->ob_val, fn->ob_size, (char*)&cfn)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return -1;
    }

    int flags = check_mode(mode, &err);
    if (err == 0) {
        return open(cfn, flags);
    }
#endif
}


FILE* pl_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
    if (fn->ob_size > 255) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return NULL;
    }

    if (mode->ob_size > 20) {
        pl_error(I, pl_static.str.ValueError, "invalid mode: %r", mode);
        return NULL;
    }

#ifdef PLATFORM_WINDOWS
    uint16_t cfn[256];
    uint16_t cmode[21];

    if (!ucs4str_to_ucs2(fn->ob_val, fn->ob_size, (uint16_t*)&cfn, false)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    if (!ucs4str_to_ucs2(mode->ob_val, mode->ob_size, (uint16_t*)&cmode, false)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid mode.");
        return NULL;
    }

    return _wfopen((const wchar_t*)cfn, (const wchar_t*)cmode);
#else
    char cfn[1536];
    char cmode[126];

    if (!ucs4str_to_utf8(fn->ob_val, fn->ob_size, (char*)&cfn, NULL)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    if (!ucs4str_to_utf8(mode->ob_val, mode->ob_size, (char*)&cmode, NULL)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid mode.");
        return NULL;
    }

    return fopen((const char*)&cfn, (const char*)&cmode);
#endif
}


PyLiteFile* pl_io_file_new(PyLiteInterpreter *I, FILE *fp) {
    PyLiteFile* mf = pylt_malloc(I, sizeof(PyLiteFile));
    mf->fp = fp;
    mf->current = 0;

    struct stat stbuf;
    fstat(fileno(fp), &stbuf);
    mf->size = stbuf.st_size;
    return mf;
}
