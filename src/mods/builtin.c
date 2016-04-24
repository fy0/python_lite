
#include "builtin.h"
#include "helper.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"

PyLiteObject* pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_import(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_mods_builtins_setattr(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_mods_builtins_gettype(PyLiteState *state, int argc, PyLiteObject **args);


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

PyLiteObject* pylt_mods_builtins_id(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, (pl_int_t)args[0]));
}

PyLiteObject* pylt_mods_builtins_hash(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, (pl_int_t)pylt_obj_chash(state, args[0])));
}

PyLiteObject* pylt_mods_builtins_len(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, (pl_int_t)pylt_obj_len(state, args[0])));
}

PyLiteObject* pylt_mods_builtins_dir(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_builtins_iter(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_iter_new(state, args[0]));
}

PyLiteObject* pylt_mods_builtins_isinstance(PyLiteState *state, int argc, PyLiteObject **args) {
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TYPE) {
        return castobj(pylt_obj_bool_new(state, pylt_api_isinstance(state, args[0], casttype(args[1])->ob_reftype)));
    }
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TUPLE) {
        for (int i = 0; i < casttuple(args[1])->ob_size; ++i) {
            PyLiteTypeObject *type = casttype(casttuple(args[1])->ob_val[i]);
            if (type->ob_type != PYLT_OBJ_TYPE_TYPE) return NULL;
            if (pylt_api_isinstance(state, args[0], type->ob_reftype)) {
                return castobj(pylt_obj_bool_new(state, true));
            }
        }
        return castobj(pylt_obj_bool_new(state, false));
    }
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

PyLiteObject* pylt_mods_builtins_gettype(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_api_gettype(state, args[0]->ob_type));
}

PyLiteModuleObject* pylt_mods_builtins_register(PyLiteState *state) {
    PyLiteModuleObject *mod = pylt_obj_module_new(state, NULL);
    static pl_uint_t print_types[] = { 0, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR };

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
        _UINTS(1, PYLT_OBJ_TYPE_STR),
        &pylt_mods_builtins_import
    );


    static pl_uint_t setattr_types[] = { PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ };

    pylt_cfunc_register(
        state,
        mod,
        _NS(state, "setattr"),
        _NST(state, 3, "object", "name", "value"),
        NULL,
        (int*)&setattr_types,
        &pylt_mods_builtins_setattr
    );

    pylt_cfunc_register(state, mod, _NS(state, "gettype"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_gettype);
    pylt_cfunc_register(state, mod, _NS(state, "id"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_id);
    pylt_cfunc_register(state, mod, _NS(state, "dir"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_dir);
    pylt_cfunc_register(state, mod, _NS(state, "len"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_len);
    pylt_cfunc_register(state, mod, _NS(state, "hash"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_hash);
    pylt_cfunc_register(state, mod, _NS(state, "iter"), _NST(state, 1, "object"), NULL, NULL, &pylt_mods_builtins_iter);
    pylt_cfunc_register(state, mod, _NS(state, "isinstance"), _NST(state, 2, "object", "class_or_type_or_tuple"), NULL, NULL, &pylt_mods_builtins_isinstance);

    pylt_mod_setattr(state, mod, "object", pylt_api_gettype(state, PYLT_OBJ_TYPE_OBJ));
    pylt_mod_setattr(state, mod, "int", pylt_api_gettype(state, PYLT_OBJ_TYPE_INT));
    pylt_mod_setattr(state, mod, "float", pylt_api_gettype(state, PYLT_OBJ_TYPE_FLOAT));
    pylt_mod_setattr(state, mod, "bool", pylt_api_gettype(state, PYLT_OBJ_TYPE_BOOL));
    pylt_mod_setattr(state, mod, "str", pylt_api_gettype(state, PYLT_OBJ_TYPE_STR));
    pylt_mod_setattr(state, mod, "bytes", pylt_api_gettype(state, PYLT_OBJ_TYPE_BYTES));
    pylt_mod_setattr(state, mod, "set", pylt_api_gettype(state, PYLT_OBJ_TYPE_SET));
    pylt_mod_setattr(state, mod, "list", pylt_api_gettype(state, PYLT_OBJ_TYPE_LIST));
    pylt_mod_setattr(state, mod, "tuple", pylt_api_gettype(state, PYLT_OBJ_TYPE_TUPLE));
    pylt_mod_setattr(state, mod, "dict", pylt_api_gettype(state, PYLT_OBJ_TYPE_DICT));

    return mod;
}
