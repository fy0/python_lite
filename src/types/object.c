
#include "../vm.h"
#include "object.h"
#include "number.h"
#include "bytes.h"
#include "bool.h"

void* basetype_op_func_table[][23] = {
    /* { LT, LE, GT, GE, NE, EQ, 
     BITOR, BITXOR, BITAND, BITLS, BITRS, 
     PLUS, MINUS, MUL, MATMUL, DIV, FLOORDIV, MOD, 
     POS, NEG, BITNOT,  POW }
    */
    { // object
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // int
        &pylt_obj_int_lt, &pylt_obj_int_le, &pylt_obj_int_gt, &pylt_obj_int_ge, &pylt_obj_int_ne, &pylt_obj_int_eq,
        &pylt_obj_int_bitor, &pylt_obj_int_bitxor, &pylt_obj_int_bitand, &pylt_obj_int_bitls, &pylt_obj_int_bitrs,
        &pylt_obj_int_plus, &pylt_obj_int_minus, &pylt_obj_int_mul, NULL, &pylt_obj_int_div, &pylt_obj_int_floordiv, &pylt_obj_int_mod,
        &pylt_obj_int_positive, &pylt_obj_int_negative, &pylt_obj_int_bitnot, &pylt_obj_int_pow
    },
    { // float
        &pylt_obj_float_lt, &pylt_obj_float_le, &pylt_obj_float_gt, &pylt_obj_float_ge, &pylt_obj_float_ne, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, &pylt_obj_float_minus, &pylt_obj_float_mul, NULL, &pylt_obj_float_div, &pylt_obj_float_floordiv, &pylt_obj_float_mod,
        &pylt_obj_float_positive, &pylt_obj_float_negative, NULL, &pylt_obj_float_pow
    },
    { // bool
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_bool_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // str
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // bytes
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_bytes_eq,
        NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // set
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // list
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // tuple
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // dict
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // function
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // module
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // type
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // class
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
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

_INLINE int pylt_obj_table_len(PyLiteTable *tab) {
    return kho_size(tab);
}

uint32_t pylt_obj_hash(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_chash(state, castint(obj));
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_chash(state, castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_hash(state, castbool(obj));
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_hash(state, castbytes(obj));
    }
    return 0;
}

uint32_t pylt_obj_hashable(PyLiteState *state, PyLiteObject *obj) {
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
            if (obj->ob_type >= PYLT_OBJ_TYPE_CLASS) {
                //pylt_obj_table_get(castclass(obj)->ob_attrs, "__hash__");
                return false;
            }
    }
    return false;
}

/* 哈希表专用函数。 */
bool pylt_obj_eq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    return (pylt_obj_op_binary(state, OP_EQ, a, b) == castobj(&PyLiteTrue));
}

uint32_t pylt_obj_istrue(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castint(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_FLOAT: return castfloat(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_BOOL: return castbool(obj)->ob_val != false;
        case PYLT_OBJ_TYPE_STR: return castbytes(obj)->ob_size != 0;
        default: return true;
    }
}

PyLiteObject* pylt_obj_op_unary(PyLiteState *state, int op, PyLiteObject *obj) {
    switch (op) {
        case OP_NOT: return castobj(pylt_obj_istrue(state, obj) ? &PyLiteFalse : &PyLiteTrue);
        default: {
            PyLiteObjUnaryOpFunc func = basetype_op_func_table[obj->ob_type - 1][op - OP_LT];
            if (func) return func(state, obj);
            return NULL;
        }
    }
}

PyLiteObject* pylt_obj_op_binary(PyLiteState *state, int op, PyLiteObject *a, PyLiteObject *b) {
    switch (op) {
        case OP_OR: return pylt_obj_istrue(state, a) ? a : b;
        case OP_AND: return pylt_obj_istrue(state, a) ? b : a;
        case OP_IN: return NULL; // TODO
        case OP_IS: return NULL; // TODO
        case OP_IS_NOT: return NULL; // TODO
        case OP_NE:
            if (a == b) return castobj(&PyLiteFalse);
            goto _def;
        case OP_EQ:
            if (a == b) return castobj(&PyLiteTrue);
            goto _def;
        default: _def: {
            PyLiteObjBinaryOpFunc func = basetype_op_func_table[a->ob_type - 1][op - OP_LT];
            if (func) return func(state, a, b);
            return NULL;
        }
    }
}

void pylt_obj_free(PyLiteObject *obj) {
    ;
}


const char* pylt_obj_basetypes[] = {
    NULL, // 0
    "object",
    "int",
    "float",
    "bool",
    "str",
    "bytes",
    "set",
    "list",
    "tuple",
    "dict",
    "function",
    "module",
    "type",
    "class",
};

const char* pylt_obj_type_name_cstr(PyLiteState *state, PyLiteObject *obj) {
    if (obj->ob_type < PYLT_OBJ_TYPE_CLASS) {
        return pylt_obj_basetypes[obj->ob_type];
    }
    return NULL;
}

const char* pylt_type_name(int ob_type) {
    return pylt_obj_basetypes[ob_type];
}
