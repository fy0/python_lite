
#include "builtin.h"
#include "../intp.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"


PyLiteObject* pylt_mods_builtins_print(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteTupleObject *values = casttuple(args[0]);

    for (int i = 0; i < values->ob_size; ++i) {
        obj = values->ob_val[i];
        if (obj->ob_type == PYLT_OBJ_TYPE_STR) {
            pylt_api_output_str(I, caststr(obj));
        } else {
            //debug_print_obj(I, obj);
            pylt_api_output_str(I, pylt_obj_to_str(I, obj));
        }
        if (i != values->ob_size - 1) pylt_api_output_str(I, caststr(args[1]));
    }

    pylt_api_output_str(I, caststr(args[2]));
    return NULL;
}


PyLiteObject* pylt_mods_builtins_id(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, (pl_int_t)args[0]));
}


PyLiteObject* pylt_mods_builtins_hash(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, (pl_int_t)pylt_obj_hash(I, args[0])));
}


PyLiteObject* pylt_mods_builtins_len(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, (pl_int_t)pylt_obj_len(I, args[0])));
}


#define get_numval(i) ((i)->ob_type == PYLT_OBJ_TYPE_FLOAT) ? (castfloat((i))->ob_val) : (castint((i))->ob_val)

PyLiteObject* pylt_mods_builtins_pow(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, pow(get_numval(args[0]), get_numval(args[1]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_builtins_dir(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteListObject *lst = pylt_obj_list_new(I);
    PyLiteTypeObject *type = pylt_api_gettype_by_code(I, args[0]->ob_type);

    if (type->ob_reftype != PYLT_OBJ_TYPE_TYPE) {
        for (pl_int32_t it = pylt_obj_dict_begin(I, type->ob_attrs); it != pylt_obj_dict_end(I, type->ob_attrs); pylt_obj_dict_next(I, type->ob_attrs, &it)) {
            pylt_obj_list_append(I, lst, pylt_obj_dict_itemkey(I, type->ob_attrs, it));
        }
    }

    switch (type->ob_reftype) {
        case PYLT_OBJ_TYPE_MODULE:
            for (pl_int32_t it = pylt_obj_dict_begin(I, castmod(args[0])->attrs); it != pylt_obj_dict_end(I, castmod(args[0])->attrs); pylt_obj_dict_next(I, castmod(args[0])->attrs, &it)) {
                pylt_obj_list_append(I, lst, pylt_obj_dict_itemkey(I, castmod(args[0])->attrs, it));
            }
            break;
        case PYLT_OBJ_TYPE_TYPE:
            for (pl_int32_t it = pylt_obj_dict_begin(I, casttype(args[0])->ob_attrs); it != pylt_obj_dict_end(I, casttype(args[0])->ob_attrs); pylt_obj_dict_next(I, casttype(args[0])->ob_attrs, &it)) {
                pylt_obj_list_append(I, lst, pylt_obj_dict_itemkey(I, casttype(args[0])->ob_attrs, it));
            }
            break;
    }

    return castobj(lst);
}

PyLiteObject* pylt_mods_builtins_iter(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_iter_new(I, args[0]));
}

PyLiteObject* pylt_mods_builtins_isinstance(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TYPE) {
        return castobj(pylt_obj_bool_new(I, pylt_api_isinstance(I, args[0], casttype(args[1])->ob_reftype)));
    }
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TUPLE) {
        for (int i = 0; i < casttuple(args[1])->ob_size; ++i) {
            PyLiteTypeObject *type = casttype(casttuple(args[1])->ob_val[i]);
            if (type->ob_type != PYLT_OBJ_TYPE_TYPE) return NULL;
            if (pylt_api_isinstance(I, args[0], type->ob_reftype)) {
                return castobj(pylt_obj_bool_new(I, true));
            }
        }
        return castobj(pylt_obj_bool_new(I, false));
    }
    return NULL;
}


PyLiteObject* pylt_mods_builtins_import(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    ;
    return NULL;
}

PyLiteObject* pylt_mods_builtins_repr(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_to_repr(I, args[0]));
}


PyLiteObject* pylt_mods_builtins_super(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pylt_api_gettype_by_code(I, args[0]->ob_type);
    return castobj(type);
}

PyLiteObject* pylt_mods_builtins_setattr(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_setattr(I, args[0], args[1], args[2]);
    return NULL;
}


PyLiteObject* pylt_mods_builtins_strtest(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_str_new_from_format(I, _NS(I, "测试%d滑%f稽%p"), pylt_obj_int_new(I, 100), pylt_obj_float_new(I, 1.2113), pylt_obj_int_new(I, 100)));
    //return castobj(_NS(I, "测试 滑稽  END"));
    return castobj(pylt_obj_to_str(I, args[0]));
}


PyLiteModuleObject* pylt_mods_builtins_register(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(builtins));

	pylt_cfunc_register(I, mod, pl_static.str.print, _NST(I, 3, "values", "sep", "end"), _NT(I, 3, castobj(&PyLiteParamArgs), _NS(I, " "), _NS(I, "\n")), _UINTS(3, NULL
		, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_print);
    pylt_cfunc_register(I, mod, pl_static.str.__import__, _NST(I, 1, "name", "globals", "locals"), NULL, _UINTS(1, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_import);
    pylt_cfunc_register(I, mod, pl_static.str.setattr, _NST(I, 3, "object", "name", "value"), NULL, _UINTS(3, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ), &pylt_mods_builtins_setattr);

    pylt_cfunc_register(I, mod, pl_static.str.id, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_id);
    pylt_cfunc_register(I, mod, pl_static.str.dir, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_dir);
    pylt_cfunc_register(I, mod, pl_static.str.len, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_len);
    pylt_cfunc_register(I, mod, pl_static.str.hash, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_hash);
    pylt_cfunc_register(I, mod, pl_static.str.iter, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_iter);
    pylt_cfunc_register(I, mod, pl_static.str.isinstance, _NST(I, 2, "object", "class_or_type_or_tuple"), NULL, NULL, &pylt_mods_builtins_isinstance);
    pylt_cfunc_register(I, mod, pl_static.str.repr, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_repr);
    pylt_cfunc_register(I, mod, pl_static.str.super, _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_super);
    pylt_cfunc_register(I, mod, pl_static.str.pow, _NT(I, 2, pl_static.str.x, pl_static.str.y), NULL, NULL, &pylt_mods_builtins_pow);
    pylt_cfunc_register(I, mod, _NS(I, "strtest"), _NT(I, 1, pl_static.str.object), NULL, NULL, &pylt_mods_builtins_strtest);

    pylt_obj_mod_setattr(I, mod, pl_static.str.None, castobj(&PyLiteNone));

	PyLiteTypeObject *type;
    for (pl_int_t i = PYLT_OBJ_TYPE_OBJ; i < I->class_num; ++i) {
		if (i >= PYLT_OBJ_TYPE_UNUSUAL && i <= PYLT_OBJ_TYPE_CODE) continue;
		if (i == PYLT_OBJ_TYPE_NONE) continue;
        type = pylt_api_gettype_by_code(I, i);
        pylt_obj_mod_setattr(I, mod, type->name, castobj(type));
    }

    return mod;
}
