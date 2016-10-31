
#include "_io.h"
#include "../bind.h"
#include "../types/all.h"

/**
class FileIO:
    pass

*/

PyLiteObject* pylt_mods_io_open(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    int len;
    PyLiteStrObject *fn = caststr(args[0]);
    PyLiteStrObject *mode = caststr(args[1]);

    char cfn[256];
    char cmode[21];

    len = ucs4str_to_utf8_size(fn->ob_val, fn->ob_size);
    if (len > 255) {
        // TODO: error
        return NULL;
    }

    len = ucs4str_to_utf8_size(mode->ob_val, mode->ob_size);
    if (len > 21) {
        // TODO: error
        return NULL;
    }

    ucs4str_to_utf8(fn->ob_val, fn->ob_size, (char*)&cfn, NULL);
    ucs4str_to_utf8(mode->ob_val, mode->ob_size, (char*)&cmode, NULL);
    FILE *fp = fopen((const char*)&cfn, (const char*)&cmode);
    return castobj(pylt_obj_cptr_new(I, fp));
}

PyLiteModuleObject* pylt_mods_io_register(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(_io));

    pylt_cfunc_register(I, mod, _NS(I, "open"), _NST(I, 2, "fn", "mode"), NULL, NULL, &pylt_mods_io_open);

    PyLiteTypeObject *type;
    type = pylt_obj_type_new(I, pl_static.str.FileIO, PYLT_OBJ_TYPE_OBJ, NULL);
    //pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_int_new);
    pylt_obj_type_register(I, type);

    return mod;
}
