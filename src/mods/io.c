
#include <errno.h>

#include "io.h"
#include "../intp.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"
#include "../utils/io.h"
#include "../utils/with_exceptions.h"

/**

class BaseIO:
    def read(self, size=None):
        pass
    def write(self, buf):
        pass

class TextIO:
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

PyLiteObject* pylt_mods_io_BaseIO_write(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_error(I, pl_static.str.NotImplementedError, NULL);
    return NULL;
}


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

    PyLiteFile *mf = (PyLiteFile*)obj->ob_ptr;
    if (mf->fno == fileno(stdin)) {
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

    PyLiteFile *mf = (PyLiteFile*)obj->ob_ptr;
    if (mf->fno == fileno(stdin)) {
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
    PyLiteCFunctionObject *func = castcfunc(I->recent_called);
    PyLiteStrObject *fn = caststr(args[0]);
    PyLiteStrObject *mode = caststr(args[1]);
    PyLiteFile *pf = pl_io_file_new(I, fn, mode);
    PyLiteModuleObject *mod = castmod(func->ob_owner);
    pl_assert(I, pl_ismod(mod), NULL);
    return castobj(pylt_obj_cptr_new(I, pf, false));
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
    pylt_type_register(I, mod, type);

    PyLiteTypeObject *tTextIO = pylt_obj_type_new(I, pl_static.str.TextIO, type->ob_reftype, NULL);
    pylt_cmethod_register(I, tTextIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    pylt_cmethod_register(I, tTextIO, _NS(I, "readline"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readline);
    pylt_type_register(I, mod, tTextIO);

    //PyLiteTypeObject *tFileIO = pylt_obj_type_new(I, pl_static.str.FileIO, type->ob_reftype, NULL);
    //pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_int_new);
    //pylt_obj_type_register(I, tFileIO);

    /*PyLiteObject *obj_stdin = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    PyLiteFile *mf = pl_io_file_new(I, stdin);
    pylt_obj_setattr(I, obj_stdin, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stdin_), obj_stdin);

    PyLiteObject *obj_stdout = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    mf = pl_io_file_new(I, stdout);
    pylt_obj_setattr(I, obj_stdout, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stdout_), obj_stdout);

    PyLiteObject *obj_stderr = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    mf = pl_io_file_new(I, stderr);
    pylt_obj_setattr(I, obj_stderr, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, mf)));
    pylt_obj_mod_setattr(I, mod, _S(stderr_), obj_stderr);*/
    
    return mod;
}
