
#include "io.h"
#include "../api.h"
#include "../intp.h"
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


PyLiteFile* pl_io_file_new(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
    int fd;
    int err = 0;
#ifdef PLATFORM_WINDOWS
    wchar_t cfn[256];
    if (fn->ob_size > 255) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return NULL;
    }

    if (!ucs4str_to_wchar(fn->ob_val, fn->ob_size, (wchar_t*)&cfn, false)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    int flags = check_mode(mode, &err);
    if (err == 0) fd = _wopen(cfn, flags);
    else {
        pl_error(I, pl_static.str.ValueError, "invalid mode: %r", mode);
        return NULL;
    }
#else
    char cfn[1536];
    if (fn->ob_size > 255) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return NULL;
    }

    if (!ucs4str_to_utf8(fn->ob_val, fn->ob_size, (char*)&cfn)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    int flags = check_mode(mode, &err);
    if (err == 0) fd = open(cfn, flags);
    else {
        pl_error(I, pl_static.str.ValueError, "invalid mode: %r", mode);
        return NULL;
    }
#endif
    if (fd != -1) {
        PyLiteFile* pf = pylt_malloc(I, sizeof(PyLiteFile));
        pf->fno = fd;
        pf->flags = flags;
        pf->current = 0;

        struct stat stbuf;
        fstat(fd, &stbuf);
        pf->size = stbuf.st_size;
        return pf;
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

PyLiteFile* pl_io_file_new_with_cfile(PyLiteInterpreter *I, FILE *fp) {
    struct stat stbuf;
    fstat(_fileno(fp), &stbuf);
    PyLiteFile *pf = pylt_malloc(I, sizeof(PyLiteFile));
    pf->fno = _fileno(fp); // st_ino is not fileno
    pf->flags = stbuf.st_mode;
    pf->size = stbuf.st_size;
    pf->current = 0;
    pf->encoding = PYLT_IOTE_BYTE;
    return pf;
}


void pl_io_init(PyLiteInterpreter *I) {
    I->sys.cin = pl_io_file_new_with_cfile(I, stdin);
    I->sys.cout = pl_io_file_new_with_cfile(I, stdout);
    I->sys.cerr = pl_io_file_new_with_cfile(I, stderr);

    I->sys.cin->encoding = PYLT_IOTE_UCS4;
    I->sys.cout->encoding = PYLT_IOTE_UCS4;
    I->sys.cerr->encoding = PYLT_IOTE_UCS4;
}

int pl_io_file_read(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count) {
    return _read(pf->fno, buf, count);
}

int pl_io_file_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count) {
    switch (pf->encoding) {
        case PYLT_IOTE_BYTE: {
            for (pl_uint_t i = 0; i < count; ++i) {
                uint8_t c = _read(pf->fno, buf, count);
                *(buf + i) = (uint32_t)c;
            }
        }
        case PYLT_IOTE_UTF8: {
            for (pl_uint_t i = 0; i < count; ++i) {
                uint8_t mybuf[6];
                _read(pf->fno, mybuf, 1);
                int chsize = utf8ch_size(mybuf[0]);
                if (chsize == 1) {
                    *(buf + i) = (uint32_t)mybuf[0];
                } else {
                    _read(pf->fno, mybuf + 1, chsize - 1);
                    utf8_decode(mybuf, buf + i);
                }
            }
        }
        case PYLT_IOTE_UCS4: {
            return _read(pf->fno, buf, count * sizeof(uint32_t));
        }
    }
    return 0;
}
