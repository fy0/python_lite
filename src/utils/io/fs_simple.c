
#include "fs_simple.h"
#include "../misc.h"
#include "../../api.h"
#include <sys/stat.h>
#include <sys/types.h>

pl_bool_t pylt_fs_simple_exists(PyLiteInterpreter *I, PyLiteStrObject *fn) {
#ifdef PLATFORM_WINDOWS
    struct _stat64i32 stbuf;
    wchar_t fn_w[PYLT_PATH_BUF_WSIZE];
    int ret = uc_ucs4str_to_utf16lez(fn->ob_val, fn->ob_size, (uint16_t*)fn_w, PYLT_PATH_BUF_WSIZE - 1);
    if (ret <= 0) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return false;
    }
    return _wstat((const wchar_t *)(fn_w), &stbuf) == 0;
#else
    char fn_u8[PYLT_PATH_BUF_SIZE];
    struct stat stbuf;
    int ret = uc_ucs4str_to_utf8z(fn->ob_val, fn->ob_size, (char*)&fn_u8, PYLT_PATH_BUF_SIZE-1);
    if (ret < 0) return ret;
    return stat(fn_u8, &stbuf) == 0;
#endif
}

pl_bool_t pylt_fs_simple_isdir(PyLiteInterpreter *I, PyLiteStrObject *fn) {
#ifdef PLATFORM_WINDOWS
    struct _stat64i32 stbuf;
    wchar_t fn_w[PYLT_PATH_BUF_WSIZE];
    int ret = uc_ucs4str_to_utf16lez(fn->ob_val, fn->ob_size, (uint16_t*)fn_w, PYLT_PATH_BUF_WSIZE - 1);
    if (ret <= 0) {
        pl_error(I, pl_static.str.ValueError, "File name too long: %r", fn);
        return false;
    }
    _wstat((const wchar_t *)(fn_w), &stbuf);
#else
    char fn_u8[PYLT_PATH_BUF_SIZE];
    struct stat stbuf;
    int ret = uc_ucs4str_to_utf8z(fn->ob_val, fn->ob_size, (char*)&fn_u8, PYLT_PATH_BUF_SIZE - 1);
    if (ret < 0) return ret;
    stat(fn_u8, &stbuf);
#endif
    return stbuf.st_mode & _S_IFDIR;
}

PyLiteFS PyLiteFSSimple = {
    .exists = &pylt_fs_simple_exists,
    .isdir = &pylt_fs_simple_isdir
};
