﻿
#include "builtin.h"
#include "helper.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"

PyLiteObject* pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_import(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_setattr(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_mods_builtins_simple_print(PyLiteState *state, int argc, PyLiteObject **args);


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
        mod,
        _NS(state, "print"),
        _NST(state, 3, "values", "sep", "end"),
        _NT(state, 3, PARAM_ARGS, _NS(state, " "), _NS(state, "\n")),
        (int*)&print_types,
        &pylt_mods_builtins_print
    );


    pylt_cfunc_register(
        mod,
        _NS(state, "__import__"),
        _NST(state, 1, "name", "globals", "locals"),
        NULL,
        _INTS(1, PYLT_OBJ_TYPE_STR),
        &pylt_mods_builtins_import
    );

    pylt_cfunc_register(
        mod,
        _NS(state, "setattr"),
        _NST(state, 3, "object", "name", "value"),
        NULL,
        _INTS(3, PTLT_OBJ_TYPE_OBJ, PYLT_OBJ_TYPE_STR, PTLT_OBJ_TYPE_OBJ),
        &pylt_mods_builtins_setattr
    );

    return mod;
}
