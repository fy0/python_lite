
#include "builtin.h"
#include "helper.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"

PyLiteObject* pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_import(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_setattr(PyLiteState *state, int argc, PyLiteObject **args);


PyLiteObject* pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteTupleObject *values = casttuple(args[0]);

    for (int i = 0; i < values->ob_size; ++i) {
        obj = values->ob_val[i];
        debug_print_obj(obj);
        if (i != values->ob_size - 1) pylt_obj_output_str(state, caststr(args[1]));
    }

    pylt_obj_output_str(state, caststr(args[2]));
    return NULL;
}


PyLiteObject* pylt_mods_builtins_import(PyLiteState *state, int argc, PyLiteObject **args) {
    ;
    return NULL;
}

PyLiteObject* pylt_mods_builtins_setattr(PyLiteState *state, int argc, PyLiteObject **args) {
    switch (args[0]->ob_type) {
        case PYLT_OBJ_TYPE_MODULE:
            pylt_obj_table_set(castmod(args[0])->attrs, args[1], args[2]);
            break;
    }
    return NULL;
}

PyLiteModuleObject* pylt_mods_builtins_register(PyLiteState *state) {
    PyLiteModuleObject *mod = pylt_obj_module_new(state, NULL);
    static int print_types[] = { 0, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR };

    pylt_cfunc_register(
        state,
        mod,
        _NS(state, "print"),
        _NST(state, 3, "values", "sep", "end"),
        _NT(state, 3, PARAM_ARGS, _NS(state, " "), _NS(state, "\n")),
        (int*)&print_types,
        &pylt_mods_builtins_print
    );


    pylt_cfunc_register(
        state,
        mod,
        _NS(state, "__import__"),
        _NST(state, 1, "name", "globals", "locals"),
        NULL,
        _INTS(1, PYLT_OBJ_TYPE_STR),
        &pylt_mods_builtins_import
    );


    static int setattr_types[] = { PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR, PTLT_OBJ_TYPE_OBJ };

    pylt_cfunc_register(
        state,
        mod,
        _NS(state, "setattr"),
        _NST(state, 3, "object", "name", "value"),
        NULL,
        (int*)&print_types,
        &pylt_mods_builtins_setattr
    );

    pylt_mod_setattr(state, mod, "object", pylt_gettype(state, PTLT_OBJ_TYPE_OBJ));
    pylt_mod_setattr(state, mod, "int", pylt_gettype(state, PTLT_OBJ_TYPE_OBJ));
    pylt_mod_setattr(state, mod, "float", pylt_gettype(state, PYLT_OBJ_TYPE_FLOAT));
    pylt_mod_setattr(state, mod, "bool", pylt_gettype(state, PYLT_OBJ_TYPE_BOOL));
    pylt_mod_setattr(state, mod, "str", pylt_gettype(state, PYLT_OBJ_TYPE_STR));
    pylt_mod_setattr(state, mod, "bytes", pylt_gettype(state, PYLT_OBJ_TYPE_BYTES));
    pylt_mod_setattr(state, mod, "set", pylt_gettype(state, PYLT_OBJ_TYPE_SET));
    pylt_mod_setattr(state, mod, "list", pylt_gettype(state, PYLT_OBJ_TYPE_LIST));
    pylt_mod_setattr(state, mod, "tuple", pylt_gettype(state, PYLT_OBJ_TYPE_TUPLE));
    pylt_mod_setattr(state, mod, "dict", pylt_gettype(state, PYLT_OBJ_TYPE_DICT));

    return mod;
}
