
#include "io.h"
#include "../misc.h"
#include "../../intp.h"
#include "../../types/all.h"

PyLiteFile* pylt_io_open(PyLiteInterpreter *I, PyLiteStrObject *fn, int mode, int encoding) {
    return pylt_io_open_full(I, I->sys.defio, fn, PYLT_IOMODE_READ, PYLT_IOTE_UTF8);
}

PyLiteFile* pylt_io_open_full(PyLiteInterpreter *I, PyLiteIO *io, PyLiteStrObject *fn, int mode, int encoding) {
    void *pfdata = io->open(I, fn, mode);
    if (I->error) return NULL;

    PyLiteFile *pf = pylt_malloc(I, sizeof(PyLiteFile));
    pf->mode = mode;
    pf->encoding = encoding;
    pf->io = io;
    pf->data = pfdata;
    return pf;
}

pl_bool_t pylt_io_readable(PyLiteInterpreter *I, PyLiteFile *pf) {
    return (pf->mode & PYLT_IOMODE_READ);
}

pl_bool_t pylt_io_writeable(PyLiteInterpreter *I, PyLiteFile *pf) {
    return (pf->mode & PYLT_IOMODE_WRITE);
}

PyLiteStrObject* pylt_io_getencoding(PyLiteInterpreter *I, PyLiteFile *pf) {
    switch (pf->encoding) {
        case PYLT_IOTE_UTF8: return pl_strnew_w(I, L"utf8", true);
        case PYLT_IOTE_UTF16: return pl_strnew_w(I, L"ucs2", true);
        case PYLT_IOTE_UCS4: return pl_strnew_w(I, L"ucs4", true);
        case PYLT_IOTE_WCHAR: return pl_strnew_w(I, L"wchar", true);
    }
    return NULL;
}

int pylt_io_read(PyLiteInterpreter *I, PyLiteFile *pf, uint8_t *buf, pl_uint_t count) {
    return pf->io->read(I, pf->data, buf, count);
}

// from encoding to ucs4
// -2 编码问题 -3 异常输入
int pylt_io_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count) {
    int read_count = 0;
    int pfencoding = pf->encoding;
    if (pfencoding == PYLT_IOTE_WCHAR) {
        if (sizeof(wchar_t) == sizeof(uint16_t)) pfencoding = PYLT_IOTE_UTF16;
        else if (sizeof(wchar_t) == sizeof(uint32_t)) pfencoding = PYLT_IOTE_UCS4;
        else return -3;
    }

    switch (pfencoding) {
        case PYLT_IOTE_UTF8: {
            int ret;
            uint8_t mybuf[6];
            for (pl_uint_t i = 0; i < count; ++i) {
                ret = pf->io->read(I, pf->data, mybuf, 1);
                if (ret < 0) return ret;
                if (ret == 0) break;
                int chsize = uc_utf8ch_size(mybuf[0]);
                if (chsize == UC_RET_INVALID) return -2;
                else if (chsize == 1) {
                    read_count += 1;
                    *(buf + i) = (uint32_t)mybuf[0];
                } else {
                    ret = pf->io->read(I, pf->data, mybuf + 1, chsize - 1);
                    if (ret < 0) return ret;
                    if (uc_ucs4_from_utf8(mybuf, chsize, buf + i) <= 0) return -2;
                    read_count += 1;
                }
            }
            return read_count;
        }
        case PYLT_IOTE_UTF16: {
            int ret;
            uint16_t u16buf[2];
            for (pl_uint_t i = 0; i < count; ++i) {
                ret = pf->io->read(I, pf->data, (uint8_t*)u16buf, sizeof(uint16_t));
                if (ret < 0) return ret;
                if (ret == 0) break;
                int chsize = uc_utf16lech_size(u16buf[0]);
                if (chsize == UC_RET_INVALID) return -2;
                else if (chsize == 1) {
                    read_count += 1;
                    *(buf + i) = (uint32_t)u16buf[0];
                } else {
                    ret = pf->io->read(I, pf->data, (uint8_t*)(u16buf + 1), sizeof(uint16_t));
                    if (ret < 0) return ret;
                    if (uc_ucs4_from_utf16le(u16buf, chsize, buf + i) <= 0) return -2;
                    read_count += 1;
                }
            }
            return read_count;
        }
        case PYLT_IOTE_UCS4: {
            return pf->io->read(I, pf->data, (uint8_t*)buf, count * sizeof(uint32_t));
        }
    }
    return -3;
}

int pylt_io_write(PyLiteInterpreter *I, PyLiteFile *pf, uint8_t *buf, pl_uint_t count) {
    return pf->io->write(I, pf->data, buf, count);
}

// ucs4 to encoding
// -2 编码问题 -3 异常输入
int pylt_io_writestr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count) {
    int written = 0;
    int pfencoding = pf->encoding;
    if (pfencoding == PYLT_IOTE_WCHAR) {
        if (sizeof(wchar_t) == sizeof(uint16_t)) pfencoding = PYLT_IOTE_UTF16;
        else if (sizeof(wchar_t) == sizeof(uint32_t)) pfencoding = PYLT_IOTE_UCS4;
        else return -3;
    }

    switch (pfencoding) {
        case PYLT_IOTE_UTF8: {
            int len;
            uint8_t u8buf[6];
            for (pl_uint_t i = 0; i < count; ++i) {
                len = uc_ucs4_to_utf8(buf[i], (uint8_t*)&u8buf);
                if (len <= 0) return -2;
                int ret = pf->io->write(I, pf->data, (uint8_t*)u8buf, len);
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
        case PYLT_IOTE_UTF16: {
            int len;
            uint16_t u16buf[2];
            for (pl_uint_t i = 0; i < count; ++i) {
                len = uc_ucs4_to_utf16le(buf[i], (uint16_t*)&u16buf);
                if (len <= 0) return -2;
                int ret = pf->io->write(I, pf->data, (uint8_t*)u16buf, len * sizeof(uint16_t));
                if (ret < 0) return ret;
                written += ret;
            }
            return written;
        }
        case PYLT_IOTE_UCS4: {
            return pf->io->write(I, pf->data, (uint8_t*)buf, count * sizeof(uint32_t));
        }
    }
    return -3;
}

int pylt_io_mode_to_posix(int mode) {
    int flag = 0;
    pl_bool_t _readable = mode & PYLT_IOMODE_READ;
    pl_bool_t _updating = mode & PYLT_IOMODE_PLUS;

    int fullflag = PYLT_IOMODE_READ | PYLT_IOMODE_WRITE | PYLT_IOMODE_APPEND | PYLT_IOMODE_CREATE;
    int keys[] = { PYLT_IOMODE_READ, PYLT_IOMODE_WRITE, PYLT_IOMODE_APPEND, PYLT_IOMODE_CREATE };
    int flags[] = { 0, O_CREAT | O_TRUNC, O_APPEND | O_CREAT, O_EXCL | O_CREAT };

    for (pl_int_t i = 0; i < sizeof(keys) / sizeof(int); ++i) {
        if (mode & keys[i]) {
            flag = flags[i];
            if (mode & (fullflag ^ keys[i])) {
                return -1;
            }
        }
    }

    if (_updating) flag |= O_RDWR;
    else if (_readable) flag |= O_RDONLY;
    else flag |= O_WRONLY;
    flag |= O_BINARY;
    return flag;
}

#define go_err(i) { \
    err = (i); \
    goto _err; \
}

int pylt_io_mode_parse(PyLiteStrObject *mode, int *perr) {
    int err = 0;
    int flags = 0;
    int base_mode = 0;
    pl_bool_t _updating = false;
    pl_bool_t _binary = false;

    for (pl_uint_t i = 0; i < mode->ob_size; ++i) {
        switch (mode->ob_val[i]) {
            case 'r': case 'w': case 'a': case 'x':
                if (base_mode) go_err(1);
                base_mode = mode->ob_val[i];
            case 'b':
                if (_binary) go_err(2);
                _binary = true;
                break;
            case '+':
                if (_updating) go_err(3);
                _updating = true;
                break;
            default:
                go_err(4);
        }
    }

    switch (base_mode) {
        case 'r': flags = PYLT_IOMODE_READ; break;
        case 'w': flags = PYLT_IOMODE_WRITE; break;
        case 'a': flags = PYLT_IOMODE_APPEND; break;
        case 'x': flags = PYLT_IOMODE_CREATE; break;
    }

    if (_updating) flags |= PYLT_IOMODE_PLUS;
    if (_binary) flags |= PYLT_IOMODE_BINARY;

    return flags;
_err:
    *perr = err;
    return 0;
}


FILE* pylt_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
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

    int ret = uc_ucs4str_to_utf16lez(fn->ob_val, fn->ob_size, (uint16_t*)&cfn, 255);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    ret = uc_ucs4str_to_utf16lez(mode->ob_val, mode->ob_size, (uint16_t*)&cmode, 20);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid mode.");
        return NULL;
    }

    return _wfopen((const wchar_t*)cfn, (const wchar_t*)cmode);
#else
    char cfn[1024];
    char cmode[21];

    int ret = uc_ucs4str_to_utf8z(fn->ob_val, fn->ob_size, (uint8_t*)&cfn, 1023);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid filename.");
        return NULL;
    }

    ret = uc_ucs4str_to_utf8z(mode->ob_val, mode->ob_size, (uint16_t*)&cmode, 20);
    if (ret < 0) {
        pl_error(I, pl_static.str.UnicodeEncodeError, "invalid mode.");
        return NULL;
    }

    return fopen((const char*)&cfn, (const char*)&cmode);
#endif
}
