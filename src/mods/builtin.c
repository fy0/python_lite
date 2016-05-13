
#include "builtin.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"


PyLiteObject* pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteTupleObject *values = casttuple(args[0]);

    for (int i = 0; i < values->ob_size; ++i) {
        obj = values->ob_val[i];
        if (obj->ob_type == PYLT_OBJ_TYPE_STR) {
            pylt_api_output_str(state, caststr(obj));
        } else {
            debug_print_obj(state, obj);
        }
        if (i != values->ob_size - 1) pylt_api_output_str(state, caststr(args[1]));
    }

    pylt_api_output_str(state, caststr(args[2]));
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


PyLiteObject* pylt_mods_builtins_fsqrt(PyLiteState *state, int argc, PyLiteObject **args) {
    if (args[0]->ob_type == PYLT_OBJ_TYPE_INT) {
        return castobj(pylt_obj_float_new(state, sqrt((double)castint(args[0])->ob_val)));
    } else if (args[0]->ob_type == PYLT_OBJ_TYPE_FLOAT) {
        return castobj(pylt_obj_float_new(state, sqrt(castfloat(args[0])->ob_val)));
    }
    return NULL;
}

PyLiteObject* pylt_mods_builtins_dir(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteListObject *lst = pylt_obj_list_new(state);
    PyLiteTypeObject *type = pylt_api_gettype(state, args[0]->ob_type);

    if (type->ob_reftype != PYLT_OBJ_TYPE_TYPE) {
        for (pl_int_t it = pylt_obj_dict_begin(state, type->ob_attrs); it != pylt_obj_dict_end(state, type->ob_attrs); pylt_obj_dict_next(state, type->ob_attrs, &it)) {
            pylt_obj_list_append(state, lst, pylt_obj_dict_itemkey(state, type->ob_attrs, it));
        }
    }

    switch (type->ob_reftype) {
        case PYLT_OBJ_TYPE_MODULE:
            break;
        case PYLT_OBJ_TYPE_TYPE:
            for (pl_int_t it = pylt_obj_dict_begin(state, casttype(args[0])->ob_attrs); it != pylt_obj_dict_end(state, casttype(args[0])->ob_attrs); pylt_obj_dict_next(state, casttype(args[0])->ob_attrs, &it)) {
                pylt_obj_list_append(state, lst, pylt_obj_dict_itemkey(state, casttype(args[0])->ob_attrs, it));
            }
            break;
    }

    return castobj(lst);
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

PyLiteObject* pylt_mods_builtins_super(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pylt_api_gettype(state, args[0]->ob_type);
    return castobj(type);
}

PyLiteObject* pylt_mods_builtins_setattr(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_setattr(state, args[0], args[1], args[2]);
    return NULL;
}


PyLiteModuleObject* pylt_mods_builtins_register(PyLiteState *state) {
    PyLiteModuleObject *mod = pylt_obj_module_new(state, NULL);

    pylt_cfunc_register(state, mod, pl_static.str.print, _NST(state, 3, "values", "sep", "end"), _NT(state, 3, PARAM_ARGS, _NS(state, " "), _NS(state, "\n")), _UINTS(3, PARAM_NODEF, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_print);
    pylt_cfunc_register(state, mod, pl_static.str.__import__, _NST(state, 1, "name", "globals", "locals"), NULL, _UINTS(1, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_import);
    pylt_cfunc_register(state, mod, pl_static.str.setattr, _NST(state, 3, "object", "name", "value"), NULL, _UINTS(3, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ), &pylt_mods_builtins_setattr);

    pylt_cfunc_register(state, mod, pl_static.str.id, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_id);
    pylt_cfunc_register(state, mod, pl_static.str.dir, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_dir);
    pylt_cfunc_register(state, mod, pl_static.str.len, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_len);
    pylt_cfunc_register(state, mod, pl_static.str.hash, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_hash);
    pylt_cfunc_register(state, mod, pl_static.str.iter, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_iter);
    pylt_cfunc_register(state, mod, pl_static.str.isinstance, _NST(state, 2, "object", "class_or_type_or_tuple"), NULL, NULL, &pylt_mods_builtins_isinstance);
    pylt_cfunc_register(state, mod, pl_static.str.super, _NT(state, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_super);
    pylt_cfunc_register(state, mod, _NS(state, "fsqrt"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_builtins_fsqrt);

    pylt_obj_mod_setattr(state, mod, pl_static.str.object, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_OBJ)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.int_, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_INT)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.float_, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_FLOAT)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.bool_, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_BOOL)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.str, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_STR)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.bytes, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_BYTES)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.set, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_SET)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.list, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_LIST)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.tuple, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_TUPLE)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.dict, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_DICT)));

    pylt_obj_mod_setattr(state, mod, pl_static.str.type, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_TYPE)));
    pylt_obj_mod_setattr(state, mod, pl_static.str.range, castobj(pylt_api_gettype(state, PYLT_OBJ_TYPE_RANGE)));

    pylt_obj_mod_setattr(state, mod, pl_static.str.None, castobj(&PyLiteNone));

    return mod;
}
