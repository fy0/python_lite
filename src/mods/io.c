
#include <errno.h>

#include "io.h"
#include "../intp.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../types/objectE.h"
#include "../utils/misc.h"
#include "../utils/io.h"

/**

class BaseIO:
    def read(self, size=None):
        pass
    def write(self, buf):
        pass

class TextIO:
    def read(self, size=None):
        pass

class BytesIO(BaseIO):
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

    pl_int_t finalcount = 0;
    uint32_t tmpbuf[8192];
    uint32_t *buf = (uint32_t*)tmpbuf;
    PyLiteFile *pf = (PyLiteFile*)obj->ob_ptr;

    if (pl_isint(args[1])) {
        pl_int_t count = castint(args[1])->ob_val;
        if (count < 8192) {
            finalcount = pl_io_file_readstr(I, pf, buf, count);
        } else {
            buf = NULL;
            pl_int_t current = 8192;
            while (true) {
                pl_int_t tmp = pl_io_file_readstr(I, pf, tmpbuf, current);
                buf = pylt_realloc(I, buf, sizeof(uint32_t) * finalcount, sizeof(uint32_t) * (finalcount + tmp));
                memcpy(buf + finalcount, tmpbuf, sizeof(uint32_t) * tmp);
                finalcount += tmp;
                if (tmp < 8192) break;
                count -= 8192;
                if (count == 0) break;
                current = min(count, 8192);
            }
        }
    } else {
        buf = NULL;
        while (true) {
            pl_int_t tmp = pl_io_file_readstr(I, pf, tmpbuf, 8192);
            buf = pylt_realloc(I, buf, sizeof(uint32_t) * finalcount, sizeof(uint32_t) * (finalcount + tmp));
            memcpy(buf + finalcount, tmpbuf, sizeof(uint32_t) * tmp);
            finalcount += tmp;
            if (tmp < 8192) break;
        }
    }
    PyLiteObject *ret = castobj(pylt_obj_str_new(I, buf, finalcount, true));
    if (buf != tmpbuf) {
        pylt_free(I, buf, sizeof(uint32_t) * finalcount);
    }
    return ret;
}

PyLiteObject* pylt_mods_io_open(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteCFunctionObject *func = castcfunc(I->recent_called);
    PyLiteFile *pf = pl_io_file_new(I, caststr(args[0]), caststr(args[1]), PYLT_IOTE_UTF8);
    if (!pf) return NULL;

    PyLiteModuleObject *mod = castmod(func->ob_owner);
    pl_assert(I, pl_ismod(mod), NULL);

    PyLiteTypeObject *iotype = casttype(pylt_obj_mod_getattr(I, mod, castobj((pf->is_binary) ? _S(BytesIO) : _S(TextIO))));
    PyLiteObject *ret = pylt_obj_cutstom_create(I, iotype->ob_reftype, NULL);
    pylt_obj_setattr(I, ret, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, pf, false)));
    return ret;
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
    pylt_cmethod_register(I, type, _NS(I, "write"), _NST(I, 2, "self", "data"), NULL, NULL, &pylt_mods_io_BaseIO_write);
    pylt_type_register(I, mod, type);

    PyLiteTypeObject *tTextIO = pylt_obj_type_new(I, pl_static.str.TextIO, type->ob_reftype, NULL);
    pylt_cmethod_register(I, tTextIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    pylt_cmethod_register(I, tTextIO, _NS(I, "readline"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readline);
    pylt_type_register(I, mod, tTextIO);

    PyLiteTypeObject *tBytesIO = pylt_obj_type_new(I, pl_static.str.BytesIO, type->ob_reftype, NULL);
    pylt_cmethod_register(I, tBytesIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    pylt_cmethod_register(I, tBytesIO, _NS(I, "readline"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readline);
    pylt_type_register(I, mod, tBytesIO);

    return mod;
}
