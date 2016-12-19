
#include "io.h"
#include "io_crt.h"
#include "../api.h"
#include "../intp.h"
#include "../types/all.h"

#include <errno.h>
#include <sys/stat.h>

#ifdef PLATFORM_WINDOWS
#include <io.h>
#include <Windows.h>
#include "../deps/linenoise/osfix/Win32_ANSI.h"
#define fileno(fp)                  _fileno(fp)
#define read(fd,buffer,count)       read(fd,buffer,count)
#define write(fd,buffer,count)      write(fd,buffer,count)
#else
#include <unistd.h>
#include <fcntl.h>
#endif

int check_mode(PyLiteStrObject *mode, int *perr, pl_bool_t *pbinary) {
    int err = 0;
    int flags = 0;
    char base_mode = 0;
    pl_bool_t _readable = false;
    pl_bool_t _updating = false;
    pl_bool_t _binary = false;

    for (pl_uint_t i = 0; i < mode->ob_size; ++i) {
        switch (mode->ob_val[i]) {
            case 'x': case 'r': case 'w': case 'a':
                if (base_mode) {
                    err = 1;
                    goto _err;
                }
                base_mode = (char)mode->ob_val[i];
                break;
            case 'b':
                if (_binary) {
                    err = 2;
                    goto _err;
                }
                _binary = true;
                break;
            case '+':
                if (_updating) {
                    err = 2;
                    goto _err;
                }
                _updating = true;
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

    if (_updating) flags |= O_RDWR;
    else if (_readable) flags |= O_RDONLY;
    else flags |= O_WRONLY;

    flags |= O_BINARY;

    if (pbinary) *pbinary = _binary;
    return flags;
_err:
    *perr = err;
    return 0;
}


PyLiteFile* pl_io_file_new(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
    int fd;
    int err = 0;
    pl_bool_t binary;
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

    int flags = check_mode(mode, &err, &binary);
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

    if (!ucs4str_to_utf8(fn->ob_val, fn->ob_size, (char*)&cfn, NULL)) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    int flags = check_mode(mode, &err, &binary);
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
    fstat(fileno(fp), &stbuf);
    PyLiteFile *pf = pylt_malloc(I, sizeof(PyLiteFile));
    pf->fno = fileno(fp); // st_ino is not fileno
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

    I->sys.cin->encoding = PYLT_IOTE_WCHAR;
    I->sys.cout->encoding = PYLT_IOTE_WCHAR;
    I->sys.cerr->encoding = PYLT_IOTE_WCHAR;
}

int pl_io_file_read(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count) {
    return read(pf->fno, buf, count);
}

// from encoding to ucs4
// -2 编码问题 -3 异常输入
int pl_io_file_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count) {
    int read_count = 0;

    pl_bool_t crt_accepted;
    int ret = crt_read(I, pf->fno, buf, count, &crt_accepted);
    if (crt_accepted) return ret;

    switch (pf->encoding) {
        case PYLT_IOTE_BYTE: {
            int ret;
            uint8_t c;
            for (pl_uint_t i = 0; i < count; ++i) {
                ret = read(pf->fno, &c, 1);
                if (ret < 0) return ret;
                read_count += ret;
                *(buf + i) = (uint32_t)c;
            }
            return read_count;
        }
        case PYLT_IOTE_UTF8: {
            int ret;
            for (pl_uint_t i = 0; i < count; ++i) {
                uint8_t mybuf[6];
                ret = read(pf->fno, mybuf, 1);
                if (ret < 0) return ret;
                read_count += ret;
                int chsize = utf8ch_size(mybuf[0]);
                if (chsize == 0) return -2;
                else if (chsize == 1) {
                    *(buf + i) = (uint32_t)mybuf[0];
                } else {
                    ret = read(pf->fno, mybuf + 1, chsize - 1);
                    if (ret < 0) return ret;
                    read_count += ret;
                    if (!utf8_decode(mybuf, buf + i)) return -2;
                }
            }
            return read_count;
        }
        case PYLT_IOTE_UCS2: {
            int ret;
            uint16_t c;
            for (pl_uint_t i = 0; i < count; ++i) {
                ret = read(pf->fno, &c, sizeof(uint16_t));
                if (ret < 0) return ret;
                read_count += ret;
                *(buf + i) = (uint32_t)c;
            }
            return read_count;
        }
        case PYLT_IOTE_UCS4: {
            return read(pf->fno, buf, count * sizeof(uint32_t));
        }
        case PYLT_IOTE_WCHAR: {
            int ret;
            wchar_t c;
            for (pl_uint_t i = 0; i < count; ++i) {
                ret = read(pf->fno, &c, sizeof(wchar_t));
                if (ret < 0) return ret;
                read_count += ret;
                *(buf + i) = (uint32_t)c;
            }
            return read_count;
        }    
    }
    return -3;
}

int pl_io_file_write(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count) {
    return write(pf->fno, buf, count);
}


// ucs4 to encoding
// -2 编码问题 -3 异常输入
int pl_io_file_writestr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count, uint32_t ignore) {
    int written = 0;

    pl_bool_t crt_accepted;
    int ret = crt_write(I, pf->fno, buf, count, ignore, &crt_accepted);
    if (crt_accepted) return ret;

    switch (pf->encoding) {
        case PYLT_IOTE_BYTE: {
            for (pl_uint_t i = 0; i < count; ++i) {
                uint8_t c8;
                uint32_t c = buf[i];
                if (c > UINT8_MAX && ignore) c = ignore;
                else return -2;
                c8 = (uint8_t)c;
                int ret = write(pf->fno, buf, 1);
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
        case PYLT_IOTE_UTF8: {
            int len;
            uint8_t mybuf[6];
            for (pl_uint_t i = 0; i < count; ++i) {
                ucs4_to_utf8(buf[i], mybuf, &len);
                int ret = write(pf->fno, mybuf, len);
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
        case PYLT_IOTE_UCS2: {
            for (pl_uint_t i = 0; i < count; ++i) {
                uint16_t c16;
                uint32_t c = buf[i];
                if (c > UINT16_MAX && ignore) c = ignore;
                else return -2;
                c16 = (uint16_t)c;
                int ret = write(pf->fno, buf, sizeof(uint16_t));
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
        case PYLT_IOTE_UCS4: {
            return write(pf->fno, buf, count * sizeof(uint32_t));
        }
        case PYLT_IOTE_WCHAR: {
            for (pl_uint_t i = 0; i < count; ++i) {
                wchar_t cw;
                uint32_t c = buf[i];
                if (c > WCHAR_MAX && ignore) c = ignore;
                else return -2;
                cw = (wchar_t)c;
                int ret = write(pf->fno, buf, sizeof(wchar_t));
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
    }
    return -3;
}
