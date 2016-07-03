
#include "../vm.h"
#include "../api.h"
#include "../misc.h"
#include "all.h"


void* basetype_op_func_table[][23] = {
    /* {
     BITOR, BITXOR, BITAND, BITLS, BITRS, 
     PLUS, MINUS, MUL, MATMUL, DIV, FLOORDIV, MOD, 
     POS, NEG, BITNOT,  POW }
    */
    { // object
        NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // int
        &pylt_obj_int_bitor, &pylt_obj_int_bitxor, &pylt_obj_int_bitand, &pylt_obj_int_bitls, &pylt_obj_int_bitrs,
        &pylt_obj_int_plus, &pylt_obj_int_minus, &pylt_obj_int_mul, NULL, &pylt_obj_int_div, &pylt_obj_int_floordiv, &pylt_obj_int_mod,
        &pylt_obj_int_positive, &pylt_obj_int_negative, &pylt_obj_int_bitnot, &pylt_obj_int_pow
    },
    { // float
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, &pylt_obj_float_minus, &pylt_obj_float_mul, NULL, &pylt_obj_float_div, &pylt_obj_float_floordiv, &pylt_obj_float_mod,
        &pylt_obj_float_positive, &pylt_obj_float_negative, NULL, &pylt_obj_float_pow
    },
    { // bool
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // str
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_str_plus, NULL, &pylt_obj_str_mul, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // bytes
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // set
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_bytes_plus, NULL, &pylt_obj_bytes_plus, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // list
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // tuple
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // dict
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // module
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // function
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // cfunction
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // code
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // type
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // iter
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // prop
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // none
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

};


/**
返回 -1: a > b
返回  0: a = b
返回 +1: a < b
返回  2: 无法比较
返回  3: False
*/
pl_int_t pylt_obj_cmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return 0;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_cmp(state, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_cmp(state, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_cmp(state, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_cmp(state, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_cmp(state, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_cmp(state, castset(a), b);
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_cmp(state, castdict(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(state, a, castobj(pl_static.str.__cmp__), NULL);
                if (hash_func) {
                    PyLiteObject *ret = pylt_vm_call_method(state, a, hash_func, 1, b);
                    if (ret->ob_type == PYLT_OBJ_TYPE_INT) {
                        return castint(ret)->ob_val;
                    }
                }
            }
    }
    return 2;
}

pl_bool_t pylt_obj_eq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return true;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_eq(state, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_eq(state, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_eq(state, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_eq(state, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_eq(state, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_eq(state, castset(a), b);
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_eq(state, castdict(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(state, a, castobj(pl_static.str.__eq__), NULL);
                if (hash_func) {
                    PyLiteObject *ret = pylt_vm_call_method(state, a, hash_func, 1, b);
                    return pylt_obj_istrue(state, ret);
                }
            }
    }
    return false;
}

pl_uint32_t pylt_obj_hash(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_hash(state, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_hash(state, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_hash(state, castbool(obj));
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_hash(state, castbytes(obj));
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_hash(state, caststr(obj));
        case PYLT_OBJ_TYPE_TYPE: return pylt_obj_type_hash(state, casttype(obj));
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *ret;
                PyLiteObject *hash_func = pylt_obj_getattr(state, obj, castobj(pl_static.str.__hash__), NULL);
                if (hash_func) {
                    ret = pylt_vm_call_method(state, obj, hash_func, 0, NULL);
                    if (ret->ob_type == PYLT_OBJ_TYPE_INT) {
                        return (pl_uint32_t)castint(ret)->ob_val;
                    }
                }
            }
    }
    return 0;
}

pl_bool_t pylt_obj_hashable(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT:
        case PYLT_OBJ_TYPE_FLOAT:
        case PYLT_OBJ_TYPE_BOOL:
        case PYLT_OBJ_TYPE_BYTES:
        case PYLT_OBJ_TYPE_STR:
        case PYLT_OBJ_TYPE_FUNCTION:
        case PYLT_OBJ_TYPE_MODULE:
        case PYLT_OBJ_TYPE_TYPE:
        case PYLT_OBJ_TYPE_TUPLE:
            return true;
        case PYLT_OBJ_TYPE_SET:
        case PYLT_OBJ_TYPE_DICT:
            return false;
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                return false;
            }
    }
    return false;
}


pl_bool_t pylt_obj_iterable(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_STR:
        case PYLT_OBJ_TYPE_BYTES:
        case PYLT_OBJ_TYPE_SET:
        case PYLT_OBJ_TYPE_LIST:
        case PYLT_OBJ_TYPE_TUPLE:
        case PYLT_OBJ_TYPE_DICT:
        case PYLT_OBJ_TYPE_RANGE:
            return true;
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *obj_func = pylt_obj_getattr(state, obj, castobj(pl_static.str.__iter__), NULL);
                return (obj_func) ? true : false;
            }
    }
    return false;
}


pl_bool_t pylt_obj_istrue(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castint(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_FLOAT: return castfloat(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_BOOL: return castbool(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_BYTES: return castbytes(obj)->ob_size != 0;
        case PYLT_OBJ_TYPE_STR: return caststr(obj)->ob_size != 0;
        default: return true;
    }
}

PyLiteObject* pylt_obj_getattr(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_MODULE:
            if (p_at_type) *p_at_type = false;
            return pylt_obj_mod_getattr(state, castmod(obj), key);
        case PYLT_OBJ_TYPE_TYPE:
            return pylt_obj_type_getattr(state, casttype(obj), key, p_at_type);
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                return pylt_obj_custom_getattr(state, castcustom(obj), key, p_at_type);
            } else {
                if (p_at_type) *p_at_type = true;
                return pylt_obj_type_getattr(state, pylt_api_gettype(state, obj->ob_type), key, NULL);
            }
            break;
    }
    return NULL;
}

pl_bool_t pylt_obj_setattr(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_TYPE:
            pylt_obj_type_setattr(state, casttype(self), key, value);
            return true;
        default:
            if (self->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *method_func = pylt_obj_getattr(state, self, castobj(pl_static.str.__setattr__), NULL);
                if (method_func) {
                    PyLiteObject *ret = pylt_vm_call_method(state, self, method_func, 2, key, value);
                    return pylt_obj_istrue(state, ret);
                } else {
                    pylt_obj_custom_setattr(state, castcustom(self), key, value);
                }
            }
            return true;
    }
    return false;
}

PyLiteObject* pylt_obj_getitem(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_bytes_getitem(state, castbytes(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_STR:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_str_getitem(state, caststr(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_LIST:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_list_getitem(state, castlist(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_TUPLE:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_tuple_getitem(state, casttuple(obj), castint(key)->ob_val));
            }
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_getitem(state, castdict(obj), key);
        default:
            if (pl_iscustom(obj)) {
                PyLiteObject *method_func = pylt_obj_getattr(state, obj, castobj(pl_static.str.__getitem__), NULL);
                if (method_func) {
                    return pylt_vm_call_method(state, obj, method_func, 1, key);
                } else {
                    if (castcustom(obj)->base_obj) {
                        return pylt_obj_getitem(state, castcustom(obj)->base_obj, key);
                    }
                }
            }
    }
    return NULL;
}

pl_bool_t pylt_obj_setitem(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_LIST:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return pylt_obj_list_setitem(state, castlist(self), castint(key)->ob_val, value);
            }
            break;
        case PYLT_OBJ_TYPE_DICT:
            pylt_obj_dict_setitem(state, castdict(self), key, value);
            return true;
    }
    return false;
}

pl_bool_t pylt_obj_has(PyLiteState *state, PyLiteObject *self, PyLiteObject *obj, pl_bool_t *is_valid) {
    if (is_valid) *is_valid = true;
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_STR:
            if (pl_isstr(obj)) {
                return pylt_obj_str_index(state, caststr(self), caststr(obj)) >= 0;
            } else {
                if (is_valid) *is_valid = false;
            }
            break;
        case PYLT_OBJ_TYPE_SET:
            return pylt_obj_set_has(state, castset(self), obj);
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_has(state, castlist(self), obj);
        case PYLT_OBJ_TYPE_TUPLE:
            break;
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_has(state, castdict(self), obj);
        default: {
            PyLiteObject *i;
            PyLiteIterObject *iter = pylt_obj_iter_new(state, self);
            if (iter) {
                for (i = pylt_obj_iter_next(state, iter); i; i = pylt_obj_iter_next(state, iter)) {
                    if (pylt_obj_eq(state, obj, i)) {
                        return true;
                    }
                }
                return false;
            } else {
                if (is_valid) *is_valid = false;
            }
        }
    }
    return false;
}

PyLiteObject* pylt_obj_op_unary(PyLiteState *state, int op, PyLiteObject *obj) {
    switch (op) {
        case OP_NOT: return castobj(pylt_obj_istrue(state, obj) ? &PyLiteFalse : &PyLiteTrue);
        default: {
            PyLiteObjUnaryOpFunc func = basetype_op_func_table[obj->ob_type - 1][op - OP_BITOR];
            if (func) return func(state, obj);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_op_binary(PyLiteState *state, int op, PyLiteObject *a, PyLiteObject *b) {
    pl_bool_t ret;
    pl_bool_t is_valid;

    switch (op) {
        case OP_OR: return pylt_obj_istrue(state, a) ? a : b;
        case OP_AND: return pylt_obj_istrue(state, a) ? b : a;
        case OP_IN:
            ret = pylt_obj_has(state, b, a, &is_valid);
            if (!is_valid) return NULL;
            return castobj(ret ? &PyLiteTrue : &PyLiteFalse);
        case OP_IS: return castobj(a == b ? &PyLiteTrue : &PyLiteFalse);
        case OP_IS_NOT: return castobj(a != b ? &PyLiteTrue : &PyLiteFalse);
        case OP_LT: case OP_LE: case OP_GT: case OP_GE:
            switch (pylt_obj_cmp(state, a, b)) {
                case -1: return castobj((op == OP_LT || op == OP_LE) ? &PyLiteTrue : &PyLiteFalse);
                case  0: return castobj((op == OP_LE || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  1: return castobj((op == OP_GT || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  3: return castobj(&PyLiteFalse);
                default: return NULL;
            }
        case OP_NE:
            return castobj(pylt_obj_eq(state, a, b) ? &PyLiteFalse : &PyLiteTrue);
        case OP_EQ:
            return castobj(pylt_obj_eq(state, a, b) ? &PyLiteTrue : &PyLiteFalse);
        default: {
            PyLiteObjBinaryOpFunc func = basetype_op_func_table[a->ob_type - 1][op - OP_BITOR];
            if (func) return func(state, a, b);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_typecast(PyLiteState *state, struct PyLiteTypeObject *type, PyLiteObject *obj) {
    if (pl_iscustom(obj) && (obj->ob_type != type->ob_reftype)) {
        return pylt_obj_cutstom_new(state, type->ob_reftype, obj);
    }
    return obj;
}

PyLiteObject* pylt_obj_getbase(PyLiteObject *obj) {
    return (pl_iscustom(obj)) ? castcustom(obj)->base_obj : obj;
}

PyLiteStrObject* pylt_obj_to_str(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return pylt_obj_int_to_str(state, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT:
            return pylt_obj_float_to_str(state, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL:
            return pylt_obj_bool_to_str(state, castbool(obj));
        case PYLT_OBJ_TYPE_STR:
            return caststr(obj);
        case PYLT_OBJ_TYPE_BYTES:
            return pylt_obj_bytes_to_str(state, castbytes(obj));
        case PYLT_OBJ_TYPE_SET:
            return pylt_obj_set_to_str(state, castset(obj));
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_to_str(state, castlist(obj));
        case PYLT_OBJ_TYPE_TUPLE:
            return pylt_obj_tuple_to_str(state, casttuple(obj));
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_to_str(state, castdict(obj));
        case PYLT_OBJ_TYPE_MODULE:
            return pylt_obj_str_new_from_format(state, pl_static.str.TMPL_MODULE_TO_STR, pl_static.str.None);
        case PYLT_OBJ_TYPE_FUNCTION:
            return pylt_obj_str_new_from_format(state, pl_static.str.TMPL_FUNCTION_TO_STR, castfunc(obj)->info.name, obj);
        case PYLT_OBJ_TYPE_CFUNCTION:
            return pylt_obj_str_new_from_format(state, pl_static.str.TMPL_CFUNCTION_TO_STR, castfunc(obj)->info.name, obj);
        case PYLT_OBJ_TYPE_NONE:
            return pl_static.str.None; 
        case PYLT_OBJ_TYPE_TYPE:
            return pylt_obj_str_new_from_format(state, pl_static.str.TMPL_CLASS_TO_STR, pylt_api_type_name(state, casttype(obj)->ob_reftype));
        default:
            // custom/range/exception 需要特别处理
            return pylt_obj_str_new_from_format(state, pl_static.str.TMPL_OBJECT_TO_STR, pylt_api_type_name(state, obj->ob_type), obj);
    }
    return NULL;
}

struct PyLiteStrObject* pylt_obj_to_repr(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_STR: {
            uint32_t *data;
            pl_uint_t i, j = 0;
            int quote_count = 0;
            PyLiteStrObject *self = caststr(obj);

            for (i = 0; i < self->ob_size; ++i) {
                if (self->ob_val[i] == '\'') quote_count++;
            }

            data = pylt_realloc(NULL, (self->ob_size + quote_count + 2) * sizeof(uint32_t));
            data[0] = '\'';
            j = 1;

            for (i = 0; i < self->ob_size; ++i) {
                if (self->ob_val[i] == '\'') {
                    data[j++] = '\\';
                }
                data[j++] = self->ob_val[i];
            }

            data[j++] = '\'';

            PyLiteStrObject *str = pylt_obj_str_new(state, data, j, true);
            pylt_free(data);
            return str;
        }
        default:
            return pylt_obj_to_str(state, obj);
    }
    return NULL;
}

void pylt_obj_free(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_OBJ:
            pylt_free(obj);
            break;
        case PYLT_OBJ_TYPE_INT:
            pylt_obj_int_free(state, castint(obj));
            break;
        case PYLT_OBJ_TYPE_FLOAT:
            pylt_obj_float_free(state, castfloat(obj));
            break;
        case PYLT_OBJ_TYPE_STR:
            pylt_obj_str_free(state, caststr(obj));
            break;
        case PYLT_OBJ_TYPE_BYTES:
            pylt_obj_bytes_free(state, castbytes(obj));
            break;
        case PYLT_OBJ_TYPE_SET:
            pylt_obj_set_free(state, castset(obj));
            break;
        case PYLT_OBJ_TYPE_LIST:
            pylt_obj_list_free(state, castlist(obj));
            break;
        case PYLT_OBJ_TYPE_TUPLE:
            pylt_obj_tuple_free(state, casttuple(obj));
            break;
        case PYLT_OBJ_TYPE_DICT:
            pylt_obj_dict_free(state, castdict(obj));
            break;
        case PYLT_OBJ_TYPE_MODULE:
            pylt_obj_module_free(state, castmod(obj));
            break;
        case PYLT_OBJ_TYPE_CODE:
            pylt_obj_code_free(state, castcode(obj));
            break;
        case PYLT_OBJ_TYPE_TYPE:
            pylt_obj_type_free(state, casttype(obj));
            break;
        case PYLT_OBJ_TYPE_ITER:
            pylt_obj_iter_free(state, castiter(obj));
            break;
        case PYLT_OBJ_TYPE_PROP:
            pylt_obj_property_free(state, castprop(obj));
            break;
        case PYLT_OBJ_TYPE_RANGE:
            pylt_obj_range_free(state, castrange(obj));
            break;
        case PYLT_OBJ_TYPE_BASE_EXCEPTION:
            pylt_obj_exception_free(state, castexcept(obj));
            break;
    }
}

void pylt_obj_safefree(PyLiteState *state, PyLiteObject *obj) {
    // TODO: fix for str
     pylt_obj_free(state, obj);
}

pl_int_t pylt_obj_len(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES: return castbytes(obj)->ob_size;
        case PYLT_OBJ_TYPE_STR: return caststr(obj)->ob_size;
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_len(state, castset(obj));
        case PYLT_OBJ_TYPE_LIST: return pylt_obj_list_count(state, castlist(obj));
        case PYLT_OBJ_TYPE_TUPLE: return casttuple(obj)->ob_size;
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_len(state, castdict(obj));
    }
    return -1;
}
