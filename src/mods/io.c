
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

#define IO_VALUE_CHK(io_ret) \
    if (io_ret == -1) { \
        pl_error(I, _S(OSError), "[Errno %d] IO Error", pylt_obj_int_new(I, errno)); \
        return NULL; \
    }

PyLiteFile* getPF(PyLiteInterpreter *I, PyLiteObject *a) {
    PyLiteCPtrObject *obj = castcptr(pylt_obj_Egetattr(I, a, castobj(_S(__cobj__)), NULL));
    if (!obj) {
        pl_error(I, pl_static.str.RuntimeError, "lost attribute: %r", _S(__cobj__));
        return NULL;
    }
    return (PyLiteFile*)obj->ob_ptr;
}

PyLiteObject* pylt_mods_io_BaseIO_read(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_error(I, pl_static.str.NotImplementedError, NULL);
    return NULL;
}

PyLiteObject* pylt_mods_io_BaseIO_write(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_error(I, pl_static.str.NotImplementedError, NULL);
    return NULL;
}

PyLiteObject* pylt_mods_io_BaseIO_fileno(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;
    return castobj(pylt_obj_int_new(I, pf->fno));
}

PyLiteObject* pylt_mods_io_BaseIO_isatty(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;
    return castobj(pylt_obj_bool_new(I, pylt_io_file_isatty(I, pf)));
}

PyLiteObject* pylt_mods_io_BaseIO_prop_encoding_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;
    return castobj(pylt_io_file_getencoding(I, pf));
}

PyLiteObject* pylt_mods_io_TextIO_readuntil(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;

    if (!(pl_isnone(args[2]) || pl_isint(args[2]))) {
        pl_error(I, pl_static.str.TypeError, "'size' must be integer or None, got %r", pl_type(I, args[2]));
        return NULL;
    }

    if (!pylt_io_file_readable(I, pf)) {
        PyLiteModuleObject *mod = pl_getmod(I, _S(io));
        pl_error_ex(I, pl_modtype(I, mod, _S(UnsupportedOperation)), "not readable");
        return NULL;
    }

    if (!pl_isstr(args[1])) {
        pl_error(I, pl_static.str.TypeError, "text must be str, got %r", pl_type(I, args[1]));
        return NULL;
    }

    pl_int_t bsize = 64;
    pl_int_t read_count = 0;
    pl_int_t maxval = pl_isnone(args[2]) ? -1 : castint(args[2])->ob_val;

    pl_int_t pindex = 0;
    uint32_t *ptext = caststr(args[1])->ob_val;

    pl_int_t index = 0;
    uint32_t *buf = (uint32_t*)pylt_malloc(I, sizeof(uint32_t) * bsize);

    while (maxval == -1 || read_count < maxval) {
        pl_int_t ret = pylt_io_file_readstr(I, pf, buf + index, 1);
        IO_VALUE_CHK(ret);
        read_count += ret;
        if (buf[index] == ptext[pindex]) {
            if (++pindex == caststr(args[1])->ob_size) break;
        } else {
            if (pindex) pindex = 0;
        }
        index++;
        if (index >= bsize) {
            buf = pylt_realloc(I, buf, sizeof(uint32_t) * bsize, sizeof(uint32_t) * (bsize + 64));
            bsize += 64;
        }
    }

    PyLiteObject *ret = castobj(pylt_obj_str_new(I, buf, read_count, true));
    pylt_free(I, buf, sizeof(uint32_t) * bsize);
    return ret;
}

PyLiteObject* pylt_mods_io_TextIO_readline(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTupleObject *t = pylt_obj_tuple_new(I, 3);
    t->ob_val[0] = args[0];
    t->ob_val[1] = castobj(pl_strnew_w(I, L"\n", true));
    t->ob_val[2] = args[1];

    PyLiteObject *ret = pylt_mods_io_TextIO_readuntil(I, 3, t->ob_val);
    pylt_obj_tuple_free(I, t);
    return ret;
}

PyLiteObject* pylt_mods_io_TextIO_read(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;

    if (!(pl_isnone(args[1]) || pl_isint(args[1]))) {
        pl_error(I, pl_static.str.TypeError, "'size' must be integer or None, got %r", pl_type(I, args[1]));
        return NULL;
    }

    pl_int_t finalcount = 0;
    uint32_t tmpbuf[8192];
    uint32_t *buf = (uint32_t*)tmpbuf;

    if (!pylt_io_file_readable(I, pf)) {
        PyLiteModuleObject *mod = pl_getmod(I, _S(io));
        pl_error_ex(I, pl_modtype(I, mod, _S(UnsupportedOperation)), "not readable");
        return NULL;
    }

    if (pl_isint(args[1])) {
        pl_int_t count = castint(args[1])->ob_val;
        if (count < 8192) {
            finalcount = pylt_io_file_readstr(I, pf, buf, count);
            IO_VALUE_CHK(finalcount);
        } else {
            buf = NULL;
            pl_int_t current = 8192;
            while (true) {
                pl_int_t tmp = pylt_io_file_readstr(I, pf, tmpbuf, current);
                if (tmp == -1) pylt_free(I, buf, sizeof(uint32_t) * finalcount);
                IO_VALUE_CHK(tmp);
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
            pl_int_t tmp = pylt_io_file_readstr(I, pf, tmpbuf, 8192);
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

PyLiteObject* pylt_mods_io_TextIO_write(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFile *pf = getPF(I, args[0]);
    if (!pf) return NULL;

    if (!pl_isstr(args[1])) {
        pl_error(I, pl_static.str.TypeError, "write() argument must be str, not %s", pl_type(I, args[1])->name);
        return NULL;
    }

    PyLiteStrObject *str = caststr(args[1]);

    if (!pylt_io_file_writeable(I, pf)) {
        PyLiteModuleObject *mod = pl_getmod(I, _S(io));
        pl_error_ex(I, pl_modtype(I, mod, _S(UnsupportedOperation)), "not writeable");
        return NULL;
    }

    return castobj(pylt_obj_int_new(I, pylt_io_file_writestr(I, pf, str->ob_val, str->ob_size, ' ')));
}

PyLiteObject* pylt_mods_io_open(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteCFunctionObject *func = castcfunc(I->recent_called);
    PyLiteFile *pf = pylt_io_file_new(I, caststr(args[0]), caststr(args[1]), PYLT_IOTE_UTF8);
    if (!pf) return NULL;

    PyLiteModuleObject *mod = castmod(func->ob_owner);
    pl_assert(I, pl_ismod(mod), NULL);

    PyLiteTypeObject *iotype = casttype(pylt_obj_mod_getattr(I, mod, (pf->is_binary) ? _S(BytesIO) : _S(TextIO)));
    PyLiteObject *ret = pylt_obj_cutstom_create(I, iotype->ob_reftype, NULL);
    pylt_obj_setattr(I, ret, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, pf, false)));
    return ret;
}


PyLiteModuleObject* pylt_mods_io_loader(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(io));

    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_CUR"), castobj(pylt_obj_int_new(I, SEEK_CUR)));
    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_END"), castobj(pylt_obj_int_new(I, SEEK_END)));
    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_SET"), castobj(pylt_obj_int_new(I, SEEK_SET)));

    pylt_cfunc_register(I, mod, _NS(I, "open"), _NST(I, 2, "fn", "mode"), NULL, NULL, &pylt_mods_io_open);

    PyLiteTypeObject *type;
    type = pylt_obj_type_new(I, pl_static.str.BaseIO, PYLT_OBJ_TYPE_OBJ, NULL);
    pylt_cmethod_register(I, type, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_BaseIO_read);
    pylt_cmethod_register(I, type, _NS(I, "write"), _NST(I, 2, "self", "data"), NULL, NULL, &pylt_mods_io_BaseIO_write);
    pylt_cmethod_register_0_args(I, type, _NS(I, "fileno"), &pylt_mods_io_BaseIO_fileno);
    pylt_cmethod_register_0_args(I, type, _NS(I, "isatty"), &pylt_mods_io_BaseIO_isatty);
    pylt_cprop_register(I, type, _S(encoding), &pylt_mods_io_BaseIO_prop_encoding_get, NULL);
    pylt_type_register(I, mod, type);

    PyLiteTypeObject *tTextIO = pylt_obj_type_new(I, pl_static.str.TextIO, type->ob_reftype, NULL);
    pylt_cmethod_register(I, tTextIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    pylt_cmethod_register(I, tTextIO, _NS(I, "readuntil"), _NST(I, 3, "self", "text", "size"), _NT(I, 3, &PyLiteParamUndefined, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readuntil);
    pylt_cmethod_register(I, tTextIO, _NS(I, "readline"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_readline);
    pylt_cmethod_register(I, tTextIO, _NS(I, "write"), _NST(I, 2, "self", "str"), NULL, NULL, &pylt_mods_io_TextIO_write);
    pylt_type_register(I, mod, tTextIO);

    PyLiteTypeObject *tBytesIO = pylt_obj_type_new(I, pl_static.str.BytesIO, type->ob_reftype, NULL);
    //pylt_cmethod_register(I, tBytesIO, _NS(I, "read"), _NST(I, 2, "self", "size"), _NT(I, 2, &PyLiteParamUndefined, &PyLiteNone), NULL, &pylt_mods_io_TextIO_read);
    //pylt_cmethod_register(I, tBytesIO, _NS(I, "write"), _NST(I, 2, "self", "str"), NULL, NULL, &pylt_mods_io_TextIO_write);
    pylt_type_register(I, mod, tBytesIO);

    PyLiteTypeObject *tOSError = casttype(pylt_obj_mod_getattr(I, pl_getmod(I, _S(pltypes)), _S(OSError)));
    PyLiteTypeObject *EUnsupportedOperation = pylt_obj_type_new(I, _S(UnsupportedOperation), tOSError->ob_reftype, NULL);
    pylt_type_register(I, mod, EUnsupportedOperation);

    return mod;
}

pl_bool_t pylt_mods_io_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(io), &pylt_mods_io_loader, true);
}
