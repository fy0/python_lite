
#include "builtin.h"
#include "../intp.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../types/objectE.h"
#include "../utils/misc.h"

#define get_val(i) ((i)->ob_type == PYLT_OBJ_TYPE_FLOAT) ? (castfloat((i))->ob_val) : (castint((i))->ob_val)

PyLiteObject* pylt_mods_builtins_abs(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isint(args[0])) {
        return castobj(pylt_obj_int_new(I, abs(castint(args[0])->ob_val)));
    }
    if (pl_isflt(args[0])) {
        return castobj(pylt_obj_float_new(I, fabs(castfloat(args[0])->ob_val)));
    }

    pl_error(I, _S(TypeError), "int or float required");
    return NULL;
}


PyLiteObject* pylt_mods_builtins_print(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteTupleObject *values = casttuple(args[0]);

    for (int i = 0; i < values->ob_size; ++i) {
        obj = values->ob_val[i];
        if (obj->ob_type == PYLT_OBJ_TYPE_STR) {
            pl_outputstr(I, caststr(obj));
        } else {
            //debug_print_obj(I, obj);
            pl_outputstr(I, pylt_obj_to_str(I, obj));
        }
        if (i != values->ob_size - 1) pl_outputstr(I, caststr(args[1]));
    }

    pl_outputstr(I, caststr(args[2]));
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

PyLiteObject* pylt_mods_builtins_next(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj = pylt_obj_iter_next(I, castiter(args[0]));
    if (!obj) {
        if (args[1] == castobj(&PyLiteUseless)) {
            pl_error(I, pl_static.str.StopIteration, NULL);
            return NULL;
        }
        return args[1];
    }
    return obj;
}

PyLiteObject* pylt_mods_builtins_dir(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteListObject *lst = pylt_obj_list_new(I);

    switch (args[0]->ob_type) {
        case PYLT_OBJ_TYPE_MODULE:
            for (pl_int32_t it = pylt_obj_dict_begin(I, castmod(args[0])->ob_attrs); it != pylt_obj_dict_end(I, castmod(args[0])->ob_attrs); pylt_obj_dict_next(I, castmod(args[0])->ob_attrs, &it)) {
                pylt_obj_list_append(I, lst, pylt_obj_dict_itemkey(I, castmod(args[0])->ob_attrs, it));
            }
            break;
        default:
            if (pl_iscustom(args[0])) {
                for (pl_int32_t it = pylt_obj_dict_begin(I, castcustom(args[0])->ob_attrs); it != pylt_obj_dict_end(I, castcustom(args[0])->ob_attrs); pylt_obj_dict_next(I, castcustom(args[0])->ob_attrs, &it)) {
                    pylt_obj_list_append(I, lst, pylt_obj_dict_itemkey(I, castcustom(args[0])->ob_attrs, it));
                }
                break;
            }
    }

    PyLiteTypeObject *type = pl_type_by_code(I, args[0]->ob_type);
    while (true) {
        for (pl_int32_t it = pylt_obj_dict_begin(I, type->ob_attrs); it != pylt_obj_dict_end(I, type->ob_attrs); pylt_obj_dict_next(I, type->ob_attrs, &it)) {
            PyLiteObject *key = pylt_obj_dict_itemkey(I, type->ob_attrs, it);
            if (!pylt_obj_list_has(I, lst, key)) {
                pylt_obj_list_append(I, lst, key);
            }
        }
        if (type->ob_reftype == PYLT_OBJ_TYPE_OBJ) break;
        type = pl_type_by_code(I, type->ob_base);
    };

    return castobj(lst);
}

PyLiteObject* pylt_mods_builtins_iter(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_iter_new(I, args[0]));
}

PyLiteObject* pylt_mods_builtins_isinstance(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TYPE) {
        return castobj(pylt_obj_bool_new(I, pl_isinstance(I, args[0], casttype(args[1])->ob_reftype)));
    }
    if (args[1]->ob_type == PYLT_OBJ_TYPE_TUPLE) {
        for (int i = 0; i < casttuple(args[1])->ob_size; ++i) {
            PyLiteTypeObject *type = casttype(casttuple(args[1])->ob_val[i]);
            if (type->ob_type != PYLT_OBJ_TYPE_TYPE) return NULL;
            if (pl_isinstance(I, args[0], type->ob_reftype)) {
                return castobj(pylt_obj_bool_new(I, true));
            }
        }
        return castobj(pylt_obj_bool_new(I, false));
    }
    return NULL;
}

PyLiteObject* pylt_mods_builtins_issubclass(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (!pl_istype(args[0])) {
        pl_error(I, _S(TypeError), "issubclass() arg 1 must be a class");
        return NULL;
    }
    if (pl_istuple(args[1])) {
        pl_foreach_tuple(I, i, args[1]) {
            if (!pl_istype(casttuple(args[1])->ob_val[i])) {
                pl_error(I, _S(TypeError), "issubclass() arg 2 must be a class or tuple of classes");
                return NULL;
            }
        }
    } else {
        if (!pl_istype(args[1])) {
            pl_error(I, _S(TypeError), "issubclass() arg 2 must be a class or tuple of classes");
            return NULL;
        }
    }

    if (pl_istuple(args[1])) {
        pl_foreach_tuple(I, i, args[1]) {
            if (pl_issubclass(I, casttype(args[0]), casttype(casttuple(args[1])->ob_val[i]))) {
                return castobj(pylt_obj_bool_new(I, true));
            }
        }
        return castobj(pylt_obj_bool_new(I, false));
    } else {
        return castobj(pylt_obj_bool_new(I, pl_issubclass(I, casttype(args[0]), casttype(args[1]))));
    }
}


PyLiteObject* pylt_mods_builtins_import(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    ;
    return NULL;
}


PyLiteObject* pylt_mods_builtins_repr(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_to_repr(I, args[0]));
}

PyLiteObject* pylt_mods_builtins_round(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // TODO: 精度支持
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, round(get_val(args[0]))));
    }

    pl_error(I, _S(TypeError), "int or float required");
    return NULL;
}


PyLiteObject* pylt_mods_builtins_super(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pl_type_by_code(I, args[0]->ob_type);
    return castobj(type);
}

PyLiteObject* pylt_mods_builtins_getattr(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return pylt_obj_Egetattr(I, args[0], args[1], NULL);
}

PyLiteObject* pylt_mods_builtins_setattr(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_Esetattr(I, args[0], args[1], args[2]);
    return NULL;
}



PyLiteModuleObject* pylt_mods_builtins_loader(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, NULL);

    pylt_cfunc_register(I, mod, _S(abs), _NST(I, 1, _S(x)), NULL, NULL, &pylt_mods_builtins_abs);
    pylt_cfunc_register(I, mod, _S(print), _NST(I, 3, "values", "sep", "end"), _NT(I, 3, castobj(&PyLiteParamArgs), _NS(I, " "), _NS(I, "\n")), _UINTS(I, 3, NULL, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_print);
    pylt_cfunc_register(I, mod, _S(__import__), _NST(I, 1, "name", "globals", "locals"), NULL, _UINTS(I, 1, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_import);
    pylt_cfunc_register(I, mod, _S(setattr), _NST(I, 3, "object", "name", "value"), NULL, _UINTS(I, 3, PYLT_OBJ_TYPE_OBJ, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ), &pylt_mods_builtins_setattr);
    pylt_cfunc_register(I, mod, _S(getattr), _NST(I, 2, "object", "name"), NULL, _UINTS(I, 2, PYLT_OBJ_TYPE_OBJ, PYLT_OBJ_TYPE_STR), &pylt_mods_builtins_getattr);
    
    pylt_cfunc_register(I, mod, _S(dir), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_dir);
    pylt_cfunc_register(I, mod, _S(id), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_id);
    pylt_cfunc_register(I, mod, _S(len), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_len);
    pylt_cfunc_register(I, mod, _S(hash), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_hash);
    pylt_cfunc_register(I, mod, _S(iter), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_iter);
    pylt_cfunc_register(I, mod, _S(next), _NT(I, 2, _S(iterator), _S(default_)), _NT(I, 2, &PyLiteParamUndefined, &PyLiteUseless), NULL, &pylt_mods_builtins_next);
    pylt_cfunc_register(I, mod, _S(isinstance), _NST(I, 2, "object", "class_or_type_or_tuple"), NULL, NULL, &pylt_mods_builtins_isinstance);
    pylt_cfunc_register(I, mod, _S(issubclass), _NST(I, 2, "class", "classinfo"), NULL, NULL, &pylt_mods_builtins_issubclass);
    pylt_cfunc_register(I, mod, _S(repr), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_repr);
    pylt_cfunc_register(I, mod, _S(round), _NST(I, 1, _S(x)), NULL, NULL, &pylt_mods_builtins_round);
    pylt_cfunc_register(I, mod, _S(super), _NT(I, 1, _S(object)), NULL, NULL, &pylt_mods_builtins_super);

    pylt_obj_mod_setattr(I, mod, pl_static.str.None, castobj(&PyLiteNone));

    PyLiteObject *key, *value;
    PyLiteModuleObject *pltypes = pl_getmod(I, _S(pltypes));
    pl_foreach_dict(I, it, pltypes->ob_attrs) {
        pylt_obj_dict_keyvalue(I, pltypes->ob_attrs, it, &key, &value);
        pylt_obj_mod_setattr(I, mod, caststr(key), value);
    }

    PyLiteModuleObject *io = pl_getmod(I, _S(io));
    pylt_obj_mod_setattr(I, mod, _S(open), pylt_obj_mod_getattr(I, io, _S(open)));

    PyLiteModuleObject *math = pl_getmod(I, _S(math));
    pylt_obj_mod_setattr(I, mod, _S(pow), pylt_obj_mod_getattr(I, io, _S(pow)));

    pylt_gc_add(I, castobj(mod));
    return mod;
}

pl_bool_t pylt_mods_builtins_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(builtins), &pylt_mods_builtins_loader, true);
}
