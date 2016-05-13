
#include "../vm.h"
#include "../api.h"
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

void pylt_obj_table_set(PyLiteTable *tab, PyLiteObject *key, PyLiteObject *val) {
    int ret;
    khiter_t k = kho_put(table, tab, key, &ret);
    kho_value(tab, k) = val;
}

PyLiteObject* pylt_obj_table_get(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kho_get(table, tab, key);
    return k == kho_end(tab) ? NULL : kho_value(tab, k);
}

bool pylt_obj_table_exists(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kho_get(table, tab, key);
    return k != kho_end(tab);
}

bool pylt_obj_table_remove(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kho_get(table, tab, key);
    if (k == kho_end(tab)) return false;
    else kho_del(table, tab, k);
    return true;
}

int pylt_obj_table_len(PyLiteTable *tab) {
    return kho_size(tab);
}

pl_int_t pylt_obj_table_begin(PyLiteTable *tab) {
    pl_int_t k = kho_begin(tab);
    while (k != kho_end(tab)) {
        if (kho_exist(tab, k)) return k;
        ++k;
    }
    return kho_end(tab);
}

pl_int_t pylt_obj_table_end(PyLiteTable *tab) {
    return kho_end(tab);
}

void pylt_obj_table_next(PyLiteTable *tab, pl_int_t *k) {
    pl_int_t key = *k;
    while (++key != kho_end(tab)) {
        if (kho_exist(tab, key)) {
            *k = key;
            return;
        }
    }
    *k = kho_end(tab);
}

PyLiteObject* pylt_obj_table_itemkey(PyLiteTable *tab, pl_int_t k) {
    return (kho_exist(tab, k)) ? castobj(kho_key(tab, k)) : NULL;
}

PyLiteObject* pylt_obj_table_itemvalue(PyLiteTable *tab, pl_int_t k) {
    return (kho_exist(tab, k)) ? castobj(kho_val(tab, k)) : NULL;
}

void pylt_obj_table_keyvalue(PyLiteTable *tab, pl_int_t k, PyLiteObject **pkey, PyLiteObject **pval) {
    if (kho_exist(tab, k)) {
        *pkey = castobj(kho_key(tab, k));
        *pval = castobj(kho_val(tab, k));
    } else {
        *pkey = NULL;
        *pval = NULL;
    }
}

/**
返回 -1: a > b
返回  0: a = b
返回 +1: a < b
返回  2: 无法比较
返回  3: False
*/
pl_int_t pylt_obj_ccmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return 0;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_ccmp(state, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_ccmp(state, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_ccmp(state, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_ccmp(state, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_ccmp(state, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_ccmp(state, castset(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(state, a, castobj(pylt_obj_str_new_from_c_str(state, "__cmp__", true)), NULL);
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

pl_bool_t pylt_obj_ceq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return true;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_ceq(state, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_ceq(state, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_ceq(state, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_ceq(state, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_ceq(state, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_ceq(state, castset(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(state, a, castobj(pylt_obj_str_new_from_c_str(state, "__eq__", true)), NULL);
                if (hash_func) {
                    PyLiteObject *ret = pylt_vm_call_method(state, a, hash_func, 1, b);
                    return pylt_obj_cistrue(state, ret);
                }
            }
    }
    return false;
}

pl_uint32_t pylt_obj_chash(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_chash(state, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_chash(state, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_chash(state, castbool(obj));
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_chash(state, castbytes(obj));
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_chash(state, caststr(obj));
        case PYLT_OBJ_TYPE_TYPE: return pylt_obj_type_chash(state, casttype(obj));
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *ret;
                PyLiteObject *hash_func = pylt_obj_getattr(state, obj, castobj(pylt_obj_str_new_from_c_str(state, "__hash__", true)), NULL);
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

pl_bool_t pylt_obj_chashable(PyLiteState *state, PyLiteObject *obj) {
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


pl_bool_t pylt_obj_citerable(PyLiteState *state, PyLiteObject *obj) {
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
                ;
            }
    }
    return false;
}


pl_bool_t pylt_obj_cistrue(PyLiteState *state, PyLiteObject *obj) {
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
                PyLiteObject *method_func = pylt_obj_getattr(state, self, castobj(pylt_obj_str_new_from_c_str(state, "__setattr__", true)), NULL);
                if (method_func) {
                    PyLiteObject *ret = pylt_vm_call_method(state, self, method_func, 2, key, value);
                    return pylt_obj_cistrue(state, ret);
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
                return castobj(pylt_obj_bytes_cgetitem(state, castbytes(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_STR:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_str_cgetitem(state, caststr(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_LIST:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_list_cgetitem(state, castlist(obj), castint(key)->ob_val));
            }
            break;
        case PYLT_OBJ_TYPE_TUPLE:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return castobj(pylt_obj_tuple_cgetitem(state, casttuple(obj), castint(key)->ob_val));
            }
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_cgetitem(state, castdict(obj), key);
    }
    return NULL;
}

pl_bool_t pylt_obj_setitem(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_LIST:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                return pylt_obj_list_csetitem(state, castlist(self), castint(key)->ob_val, value);
            }
            break;
        case PYLT_OBJ_TYPE_DICT:
            pylt_obj_dict_csetitem(state, castdict(self), key, value);
            return true;
    }
    return false;
}

PyLiteObject* pylt_obj_op_unary(PyLiteState *state, int op, PyLiteObject *obj) {
    switch (op) {
        case OP_NOT: return castobj(pylt_obj_cistrue(state, obj) ? &PyLiteFalse : &PyLiteTrue);
        default: {
            PyLiteObjUnaryOpFunc func = basetype_op_func_table[obj->ob_type - 1][op - OP_BITOR];
            if (func) return func(state, obj);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_op_binary(PyLiteState *state, int op, PyLiteObject *a, PyLiteObject *b) {
    switch (op) {
        case OP_OR: return pylt_obj_cistrue(state, a) ? a : b;
        case OP_AND: return pylt_obj_cistrue(state, a) ? b : a;
        case OP_IN: return NULL; // TODO
        case OP_IS: return castobj(a == b ? &PyLiteTrue : &PyLiteFalse);
        case OP_IS_NOT: return castobj(a != b ? &PyLiteTrue : &PyLiteFalse);
        case OP_LT: case OP_LE: case OP_GT: case OP_GE:
            switch (pylt_obj_ccmp(state, a, b)) {
                case -1: return castobj((op == OP_LT || op == OP_LE) ? &PyLiteTrue : &PyLiteFalse);
                case  0: return castobj((op == OP_LE || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  1: return castobj((op == OP_GT || op == OP_GE) ? &PyLiteTrue : &PyLiteFalse);
                case  3: return castobj(&PyLiteFalse);
                default: return NULL;
            }
        case OP_NE:
            return castobj(pylt_obj_ceq(state, a, b) ? &PyLiteFalse : &PyLiteTrue);
        case OP_EQ:
            return castobj(pylt_obj_ceq(state, a, b) ? &PyLiteTrue : &PyLiteFalse);
        default: {
            PyLiteObjBinaryOpFunc func = basetype_op_func_table[a->ob_type - 1][op - OP_BITOR];
            if (func) return func(state, a, b);
            return NULL;
        }
    }
}

void pylt_obj_free(PyLiteObject *obj) {
    ;
}

pl_int_t pylt_obj_len(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES: return castbytes(obj)->ob_size;
        case PYLT_OBJ_TYPE_STR: return caststr(obj)->ob_size;
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_len(state, castset(obj));
        case PYLT_OBJ_TYPE_LIST: return pylt_obj_list_ccount(state, castlist(obj));
        case PYLT_OBJ_TYPE_TUPLE: return casttuple(obj)->ob_size;
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_len(state, castdict(obj));
    }
    return -1;
}
