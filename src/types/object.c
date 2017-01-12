
#include "all.h"
#include "../vm.h"
#include "../api.h"
#include "../utils/misc.h"
#include "objectE.h"


void* basetype_op_func_table[][24] = {
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
        &pylt_obj_str_plus, NULL, &pylt_obj_str_mul, NULL, NULL, NULL, &pylt_obj_str_mod,
        NULL, NULL, NULL, NULL
    },
    { // bytes
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_bytes_plus, NULL, &pylt_obj_bytes_plus, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // set
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // list
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // tuple
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // dict
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // unusual
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // module
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // function
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
返回  4: 无cmp函数
*/
pl_int_t pylt_obj_cmp(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return 0;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_cmp(I, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_cmp(I, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_cmp(I, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_cmp(I, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_cmp(I, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_cmp(I, castset(a), b);
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_cmp(I, castdict(a), b);
        default: return 4;
    }
    return 2;
}

pl_bool_t pylt_obj_eq(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return true;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_eq(I, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_eq(I, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_eq(I, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_eq(I, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_eq(I, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_eq(I, castset(a), b);
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_eq(I, castdict(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(I, a, castobj(pl_static.str.__eq__), NULL);
                if (hash_func) {
                    PyLiteObject *ret = pl_call_method(I, a, hash_func, 1, b);
                    return pylt_obj_istrue(I, ret);
                }
            }
    }
    return false;
}

// 这个可怜的函数无法添加更多参数了，因为他被 khash 所用。
// __hash__ 的错误无法捕获，所以不对外提供。
pl_uint32_t pylt_obj_hash(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_hash(I, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_hash(I, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_hash(I, castbool(obj));
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_hash(I, castbytes(obj));
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_hash(I, caststr(obj));
        case PYLT_OBJ_TYPE_TYPE: return pylt_obj_type_hash(I, casttype(obj));
        /*default:
            if (pl_iscustom(obj)) {
                PyLiteObject *ret;
                PyLiteObject *hash_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__hash__), NULL);
                if (hash_func) {
                    ret = pl_call_method(I, obj, hash_func, 0, NULL);
                    if (ret->ob_type == PYLT_OBJ_TYPE_INT) {
                        return (pl_uint32_t)castint(ret)->ob_val;
                    }
                }
            }
            return (pl_uint32_t)obj;*/
    }
    return 0;
}

pl_bool_t pylt_obj_hashable(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_SET:
        case PYLT_OBJ_TYPE_DICT:
        case PYLT_OBJ_TYPE_UNUSUAL:
            return false;
        default:
            /*if (pl_iscustom(obj)) {
                PyLiteObject *method_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__hash__), NULL);
                if (method_func) return true;
                return pylt_obj_hashable(I, castcustom(obj)->base_obj);
            }*/
            return pl_iscustom(obj) ? false : true;
    }
}


pl_bool_t pylt_obj_iterable(PyLiteInterpreter *I, PyLiteObject *obj) {
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
                PyLiteObject *obj_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__iter__), NULL);
                if (obj_func) return true;
                return pylt_obj_iterable(I, castcustom(obj)->base_obj);
            }
    }
    return false;
}


pl_bool_t pylt_obj_istrue(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castint(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_FLOAT: return castfloat(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_BOOL: return castbool(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_STR: return caststr(obj)->ob_size != 0;
        case PYLT_OBJ_TYPE_BYTES: return castbytes(obj)->ob_size != 0;
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_len(I, castset(obj)) != 0;
        case PYLT_OBJ_TYPE_LIST: return pylt_obj_list_count(I, castlist(obj)) != 0;
        case PYLT_OBJ_TYPE_TUPLE: return casttuple(obj)->ob_size != 0;
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_len(I, castdict(obj)) != 0;
        case PYLT_OBJ_TYPE_NONE: return false;
        default: {
            if (pl_iscustom(obj)) return pylt_obj_istrue(I, castcustom(obj)->base_obj);
            return true;
        }
    }
}

PyLiteObject* pylt_obj_getattr_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, PyLiteObject* _default, pl_bool_t *p_at_type) {
    PyLiteObject *ret = NULL;
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_MODULE:
            if (p_at_type) *p_at_type = false;
            ret = pylt_obj_mod_getattr(I, castmod(obj), caststr(key));
            break;
        case PYLT_OBJ_TYPE_TYPE:
            ret = pylt_obj_type_getattr(I, casttype(obj), key, p_at_type);
            break;
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                ret = pylt_obj_custom_getattr(I, castcustom(obj), key, p_at_type);
            } else {
                if (p_at_type) *p_at_type = true;
                ret = pylt_obj_type_getattr(I, pl_type_by_code(I, obj->ob_type), key, NULL);
            }
            break;
    }
    return ret;
}

PyLiteObject* pylt_obj_getattr(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type) {
    PyLiteObject *ret = pylt_obj_getattr_ex(I, obj, key, castobj(&PyLiteUseless), p_at_type);
    return (ret != castobj(&PyLiteUseless)) ? ret : NULL;
}

pl_bool_t pylt_obj_setattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_TYPE:
            pylt_obj_type_setattr(I, casttype(self), key, value);
            return true;
        default:
            // TODO: 对内置类型做处理
            if (pl_iscustom(self)) {
                pylt_obj_custom_setattr(I, castcustom(self), key, value);
                return true;
            }
    }
    return false;
}

pl_bool_t pylt_obj_delattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_TYPE:
            pylt_obj_type_delattr(I, casttype(self), key);
            return true;
        default:
            if (pl_iscustom(self)) {
                pylt_obj_custom_delattr(I, castcustom(self), key);
                return true;
            }
    }
    return false;
}

pl_bool_t pylt_obj_has(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *obj, pl_bool_t *is_valid) {
    if (is_valid) *is_valid = true;
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_STR:
            if (pl_isstr(obj)) {
                return pylt_obj_str_index(I, caststr(self), caststr(obj)) >= 0;
            } else {
                if (is_valid) *is_valid = false;
            }
            break;
        case PYLT_OBJ_TYPE_SET:
            return pylt_obj_set_has(I, castset(self), obj);
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_has(I, castlist(self), obj);
        case PYLT_OBJ_TYPE_TUPLE:
            break;
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_has(I, castdict(self), obj);
        default: {
            PyLiteObject *i;
            PyLiteIterObject *iter = pylt_obj_iter_new(I, self);
            if (iter) {
                for (i = pylt_obj_iter_next(I, iter); i; i = pylt_obj_iter_next(I, iter)) {
                    if (pylt_obj_eq(I, obj, i)) {
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

PyLiteObject* pylt_obj_op_unary(PyLiteInterpreter *I, int op, PyLiteObject *obj) {
    switch (op) {
        case OP_NOT: return castobj(pylt_obj_istrue(I, obj) ? &PyLiteFalse : &PyLiteTrue);
        default: {
            PyLiteObjUnaryOpFunc func = basetype_op_func_table[obj->ob_type - 1][op - OP_BITOR];
            if (func) return func(I, obj);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_op_binary(PyLiteInterpreter *I, int op, PyLiteObject *a, PyLiteObject *b) {
    pl_bool_t ret;
    pl_bool_t is_valid;

    switch (op) {
        case OP_OR: return pylt_obj_istrue(I, a) ? a : b;
        case OP_AND: return pylt_obj_istrue(I, a) ? b : a;
        case OP_IN:
            ret = pylt_obj_has(I, b, a, &is_valid);
            if (!is_valid) return NULL;
            return castobj(ret ? &PyLiteTrue : &PyLiteFalse);
        case OP_NOT_IN:
            ret = pylt_obj_has(I, b, a, &is_valid);
            if (!is_valid) return NULL;
            return castobj(ret ? &PyLiteFalse : &PyLiteTrue);
        case OP_IS: return castobj(a == b ? &PyLiteTrue : &PyLiteFalse);
        case OP_IS_NOT: return castobj(a != b ? &PyLiteTrue : &PyLiteFalse);
        case OP_LT: case OP_LE: case OP_GT: case OP_GE:
            switch (pylt_obj_cmp(I, a, b)) {
                case -1: return castobj((op == OP_LT || op == OP_LE) ? &PyLiteTrue : &PyLiteFalse);
                case  0: return castobj((op == OP_LE || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  1: return castobj((op == OP_GT || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  3: return castobj(&PyLiteFalse);
                default: return NULL;
            }
        case OP_NE:
            return castobj(pylt_obj_eq(I, a, b) ? &PyLiteFalse : &PyLiteTrue);
        case OP_EQ:
            return castobj(pylt_obj_eq(I, a, b) ? &PyLiteTrue : &PyLiteFalse);
        default: {
            PyLiteObjBinaryOpFunc func = basetype_op_func_table[a->ob_type - 1][op - OP_BITOR];
            if (func) return func(I, a, b);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_getbase(PyLiteObject *obj) {
    return (pl_iscustom(obj)) ? castcustom(obj)->base_obj : obj;
}

PyLiteStrObject* pylt_obj_to_str(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return pylt_obj_int_to_str(I, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT:
            return pylt_obj_float_to_str(I, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL:
            return pylt_obj_bool_to_str(I, castbool(obj));
        case PYLT_OBJ_TYPE_STR:
            return caststr(obj);
        case PYLT_OBJ_TYPE_BYTES:
            return pylt_obj_bytes_to_str(I, castbytes(obj));
        case PYLT_OBJ_TYPE_SET:
            return pylt_obj_set_to_str(I, castset(obj));
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_to_str(I, castlist(obj));
        case PYLT_OBJ_TYPE_TUPLE:
            return pylt_obj_tuple_to_str(I, casttuple(obj));
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_to_str(I, castdict(obj));
        case PYLT_OBJ_TYPE_UNUSUAL:
            return pylt_obj_unusual_to_str(I, castunusual(obj));
        case PYLT_OBJ_TYPE_MODULE:
            return pl_format(I, pl_static.str.TMPL_MODULE_TO_STR, castmod(obj)->name ? castmod(obj)->name : pl_cformat(I, "<unknown>"));
        case PYLT_OBJ_TYPE_FUNCTION:
            return pl_format(I, pl_static.str.TMPL_FUNCTION_TO_STR, castfunc(obj)->info.name, obj);
        case PYLT_OBJ_TYPE_CFUNCTION:
            return pl_format(I, pl_static.str.TMPL_CFUNCTION_TO_STR, castcfunc(obj)->info.name, obj);
        case PYLT_OBJ_TYPE_NONE:
            return pl_static.str.None;
        case PYLT_OBJ_TYPE_TYPE:
            return pl_format(I, pl_static.str.TMPL_CLASS_TO_STR, pl_type_by_code(I, casttype(obj)->ob_reftype)->name);
        default:
            // TODO: range/exception 也需要折腾
            return pl_format(I, pl_static.str.TMPL_OBJECT_TO_STR, pl_type(I, obj)->name, obj);
    }
    return NULL;
}

struct PyLiteStrObject* pylt_obj_to_repr(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_to_repr(I, caststr(obj));
        default: return pylt_obj_to_str(I, obj);
    }
    return NULL;
}

void pylt_obj_free(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_OBJ:
            pylt_free_ex(I, obj);
            break;
        case PYLT_OBJ_TYPE_INT:
            pylt_obj_int_free(I, castint(obj));
            break;
        case PYLT_OBJ_TYPE_FLOAT:
            pylt_obj_float_free(I, castfloat(obj));
            break;
        case PYLT_OBJ_TYPE_STR:
            pylt_obj_str_free(I, caststr(obj));
            break;
        case PYLT_OBJ_TYPE_BYTES:
            pylt_obj_bytes_free(I, castbytes(obj));
            break;
        case PYLT_OBJ_TYPE_SET:
            pylt_obj_set_free(I, castset(obj));
            break;
        case PYLT_OBJ_TYPE_LIST:
            pylt_obj_list_free(I, castlist(obj));
            break;
        case PYLT_OBJ_TYPE_TUPLE:
            pylt_obj_tuple_free(I, casttuple(obj));
            break;
        case PYLT_OBJ_TYPE_DICT:
            pylt_obj_dict_free(I, castdict(obj));
            break;
        case PYLT_OBJ_TYPE_MODULE:
            pylt_obj_module_free(I, castmod(obj));
            break;
        case PYLT_OBJ_TYPE_CODE:
            pylt_obj_code_free(I, castcode(obj));
            break;
        case PYLT_OBJ_TYPE_TYPE:
            pylt_obj_type_free(I, casttype(obj));
            break;
        case PYLT_OBJ_TYPE_ITER:
            pylt_obj_iter_free(I, castiter(obj));
            break;
        case PYLT_OBJ_TYPE_PROP:
            pylt_obj_property_free(I, castprop(obj));
            break;
        case PYLT_OBJ_TYPE_RANGE:
            pylt_obj_range_free(I, castrange(obj));
            break;
        case PYLT_OBJ_TYPE_BASE_EXCEPTION:
            pylt_obj_exception_free(I, castexcept(obj));
            break;
    }
}

pl_int_t pylt_obj_len(PyLiteInterpreter *I, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES: return castbytes(obj)->ob_size;
        case PYLT_OBJ_TYPE_STR: return caststr(obj)->ob_size;
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_len(I, castset(obj));
        case PYLT_OBJ_TYPE_LIST: return pylt_obj_list_count(I, castlist(obj));
        case PYLT_OBJ_TYPE_TUPLE: return casttuple(obj)->ob_size;
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_len(I, castdict(obj));
    }
    return -1;
}
