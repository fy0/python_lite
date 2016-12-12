
#include <errno.h>
#include <sys/stat.h>

#include "io.h"
#include "cio.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"
#include "../utils/with_exceptions.h"

/**

class BaseIO:
    def read(self, size=None):
        pass
    def write(self, buf):
        pass

class TextStdinIO:
    def read(self, size=None):
        pass

class FileIO(BaseIO):
    def readline(self, size=-1):
        pass

*/

PyLiteObject* pylt_mods_io_BaseIO_read(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_error(I, pl_static.str.NotImplementedError, NULL);
    return NULL;
}

typedef struct MyFile {
    FILE *fp;
    pl_uint_t current;
    pl_uint_t size;
} MyFile;

struct MyFile* myfile_new(PyLiteInterpreter *I, FILE *fp) {
    struct MyFile* mf = pylt_malloc(I, sizeof(struct MyFile));
    mf->fp = fp;
    mf->current = 0;

    struct stat stbuf;
    fstat(fileno(fp), &stbuf);
    mf->size = stbuf.st_size;
    return mf;
}

#include "../deps/linenoise/osfix/_osfix.h"

PyLiteObject* pylt_mods_io_TextIO_readline(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteCPtrObject *obj = castcptr(pylt_obj_Egetattr(I, args[0], castobj(_S(__cobj__)), NULL));
    if (!obj) {
        pl_error(I, pl_static.str.RuntimeError, "lost attribute: %r", _S(__cobj__));
        return NULL;
    }

    if (!(pl_isnone(args[1]) || pl_isint(args[1]))) {
        pl_print(I, "%s\n", args[1]);
        pl_error(I, pl_static.str.TypeError, "'size' must be integer or None, got %r", pl_type(I, args[1]));
        return NULL;
    }

    MyFile *mf = (MyFile*)obj->ob_ptr;
    if (mf->fp == stdin) {
        pl_int_t count = pl_isnone(args[1]) ? 8192 : min(castint(args[1])->ob_val, 8192);
        uint32_t buf[8192];
        uint32_t *s = (uint32_t*)&buf;
        for (pl_int_t i = 0; i < count; ++i) {
            *s = (uint32_t)fgetwc(stdin);
            if (*s == '\n') {
                count = i + 1;
                break;
            }
            s++;
        }
        return castobj(pylt_obj_str_new(I, buf, count, true));
    }

    return NULL;
}

PyLiteObject* pylt_mods_io_TextIO_read(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteCPtrObject *obj = castcptr(pylt_obj_Egetattr(I, args[0], castobj(_S(__cobj__)), NULL));
    if (!obj) {
        pl_error(I, pl_static.str.RuntimeError, "lost attribute: %r", _S(__cobj__));
        return NULL;
    }

    if (!(pl_isnone(args[1]) || pl_isint(args[1]))) {
        pl_print(I, "%s\n", args[1]);
        pl_error(I, pl_static.str.TypeError, "'size' must be integer or None, got %r", pl_type(I, args[1]));
        return NULL;
    }

    MyFile *mf = (MyFile*)obj->ob_ptr;
    if (mf->fp == stdin) {
        pl_int_t count = pl_isnone(args[1]) ? 8192 : min(castint(args[1])->ob_val, 8192);
        uint32_t buf[8192];
        uint32_t *s = (uint32_t*)&buf;
        for (pl_int_t i = 0; i < count; ++i) {
            *s++ = (uint32_t)fgetwc(stdin);
        }
        return castobj(pylt_obj_str_new(I, buf, count, true));
    }

    return NULL;
}

PyLiteObject* pylt_mods_io_open(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteStrObject *fn = caststr(args[0]);
    PyLiteStrObject *mode = caststr(args[1]);
    FILE *fp = mfopen(I, fn, mode);

    pl_bool_t is_bin = false;
    for (pl_uint_t i = 0; i <= mode->ob_size; ++i) {
        if (mode->ob_val[i] == 'b') {
            is_bin = true;
            break;
        }
    }

    if (!is_bin) {
        mode = caststr(pylt_obj_str_plus(I, mode, castobj(pylt_obj_str_new_from_cstr(I, "b", true))));
    }

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
    type = pylt_obj_type_new(I, pl_static.str.BaseIO, PYLT_OBJ_TYPE_OBJ, NULL);
    pylt_cmethod_register(I, type, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_BaseIO_read);
    pylt_cmethod_register(I, type, _NS(I, "write"), _NST(I, 2, "self", "data"), NULL, NULL, &pylt_mods_io_BaseIO_read);
    pylt_obj_type_register(I, type);

    PyLiteTypeObject *tTextIO = pylt_obj_type_new(I, pl_static.str.TextIO, type->ob_reftype, NULL);
    pylt_cmethod_register(I, tTextIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    pylt_cmethod_register(I, tTextIO, _NS(I, "readline"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readline);
    pylt_obj_type_register(I, tTextIO);

    //PyLiteTypeObject *tFileIO = pylt_obj_type_new(I, pl_static.str.FileIO, type->ob_reftype, NULL);
    //pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_int_new);
    //pylt_obj_type_register(I, tFileIO);

    PyLiteObject *obj_stdin = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    struct MyFile *mf = myfile_new(I, stdin);
    pylt_obj_setattr(I, obj_stdin, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stdin_), obj_stdin);

    PyLiteObject *obj_stdout = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    mf = myfile_new(I, stdout);
    pylt_obj_setattr(I, obj_stdout, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stdout_), obj_stdout);

    PyLiteObject *obj_stderr = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    mf = myfile_new(I, stderr);
    pylt_obj_setattr(I, obj_stderr, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stderr_), obj_stderr);

    return mod;
}
