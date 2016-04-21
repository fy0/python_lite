
#include "builtin.h"
#include "../state.h"
#include "../debug.h"
#include "helper.h"

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
        if (i != values->ob_size - 1) printf(" ");
    }

    putchar('\n');
    return NULL;
}

PyLiteObject* pylt_mods_builtins_simple_print(PyLiteState *state, int argc, PyLiteObject **args) {
    if (argc) debug_print_obj(args[0]);
    putchar('\n');
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

    pylt_cfunc_register(
        mod,
        _NS(state, "print"),
        _NST(state, 3, "values", "sep", "end"),
        _NT(state, 3, PARAM_ARGS, _NS(state, " "), _NS(state, "\n")),
        NULL, 
        &pylt_mods_builtins_print
    );

    pylt_cfunc_register(
        mod,
        _NS(state, "simple_print"),
        _NST(state, 1, "values"),
        NULL,
        NULL,
        &pylt_mods_builtins_simple_print
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
