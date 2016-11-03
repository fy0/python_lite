
#include <errno.h>
#include <sys/stat.h>

#include "io.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"

/**
class FileIO:
    def read(size):
        pass
    def readline(size=-1):
        pass

*/

struct MyFile {
    FILE *fp;
    pl_uint_t current;
    pl_uint_t size;
};

FILE* mfopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode) {
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

PyLiteObject* pylt_mods_io_open(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteStrObject *fn = caststr(args[0]);
    PyLiteStrObject *mode = caststr(args[1]);
    FILE *fp = mfopen(I, fn, mode);

    if (!fp) {
        switch (errno) {
            case ENOENT:
                pl_error(I, pl_static.str.FileNotFoundError, "[Errno 2] No such file or directory: %r", args[0]);
                break;
            case EACCES: // open dir, windows
                pl_error(I, pl_static.str.PermissionError, "[Errno 13] Permission denied: %r", args[0]);
                break;
            case EISDIR: // open dir, linux
                pl_error(I, pl_static.str.IsADirectoryError, "[Errno 21] Is a directory: %r", args[0]);
                break;
            default:
                pl_error(I, pl_static.str.OSError, "[Errno %d] File Open Error: %r", pylt_obj_int_new(I, errno), args[0]);
        }
    }

    struct stat stbuf;
    fstat(fileno(fp), &stbuf);

    return castobj(pylt_obj_cptr_new(I, fp));
}


PyLiteObject* pylt_mods_io_read(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_int_t size = castint(args[1])->ob_val;
    uint8_t *buf = pylt_malloc(I, size);
    size_t read = fread(buf, size, 1, castcptr(args[0])->ob_ptr);
    if (read == 0) return NULL;
    else {
        return castobj(pylt_obj_bytes_new(I, (const char*)buf, size, true));
    }
    return NULL;
}

PyLiteModuleObject* pylt_mods_io_register(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(io));

    pylt_cfunc_register(I, mod, _NS(I, "open"), _NST(I, 2, "fn", "mode"), NULL, NULL, &pylt_mods_io_open);
    pylt_cfunc_register(I, mod, _NS(I, "read"), _NST(I, 2, "file", "size"), _NT(I, 2, &PyLiteParamUndefined, pylt_obj_int_new(I, -1)), NULL, &pylt_mods_io_read);

    PyLiteTypeObject *type;
    type = pylt_obj_type_new(I, pl_static.str.FileIO, PYLT_OBJ_TYPE_OBJ, NULL);
    //pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_int_new);
    pylt_obj_type_register(I, type);

    return mod;
}
