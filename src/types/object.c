
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
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_int_eq,
        NULL, NULL, NULL, NULL, NULL, 
        &pylt_obj_int_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // float
        NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL, 
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    
};

void pylt_obj_table_set(PyLiteTable *tab, PyLiteObject *key, PyLiteObject *val) {
    int ret;
    khiter_t k = kh_put(table, tab, key, &ret);
    kh_value(tab, k) = val;
}

PyLiteObject* pylt_obj_table_get(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kh_get(table, tab, key);
    return k == kh_end(tab) ? NULL : kh_value(tab, k);
}

bool pylt_obj_table_exists(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kh_get(table, tab, key);
    return k != kh_end(tab);
}

bool pylt_obj_table_remove(PyLiteTable *tab, PyLiteObject *key) {
    khiter_t k = kh_get(table, tab, key);
    if (k == kh_end(tab)) return false;
    else kh_del(table, tab, k);
    return true;
}

_INLINE
int pylt_obj_table_len(PyLiteTable *tab) {
    return kh_size(tab);
}

uint32_t pylt_obj_hash(PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_hash(castint(obj));
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_hash(castfloat(obj));
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_hash(castbool(obj));
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_hash(castbytes(obj));
    }
    return 0;
}

uint32_t pylt_obj_hashable(PyLiteObject *obj) {
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

/* 思路：这个函数并不是二元运算符的映射，而是主要用于哈希表。 */
bool pylt_obj_eq(PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return true;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_eq(castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_eq(castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_eq(castbool(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_eq(castbytes(a), b);
    }
    return false;
}

uint32_t pylt_obj_istrue(PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castint(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_FLOAT: return castfloat(obj)->ob_val != 0;
        case PYLT_OBJ_TYPE_BOOL: return castbool(obj)->ob_val != false;
        case PYLT_OBJ_TYPE_STR: return castbytes(obj)->ob_size != 0;
        default: return true;
    }
}

PyLiteObject* pylt_obj_op_binary(int op, PyLiteObject *a, PyLiteObject *b) {
    switch (op) {
        case OP_OR: return pylt_obj_istrue(a) ? a : b;
        case OP_AND: return pylt_obj_istrue(a) ? b : a;
        case OP_IN: return NULL; // TODO
        case OP_IS: return NULL; // TODO
        case OP_IS_NOT: return NULL; // TODO
        case OP_NE:
            if (a == b) return &PyLiteFalse;
            goto _def;
        case OP_EQ:
            if (a == b) return &PyLiteTrue;
            goto _def;
        default: _def: {
            PyLiteObjBinaryOpFunc func = basetype_op_func_table[a->ob_type - 1][op - OP_LT];
            if (func) return func(a, b);
            return NULL;
        }
    }
}

void pylt_obj_free(PyLiteObject *obj) {
    ;
}


const char* pylt_obj_basetypes[] = {
    NULL, // 0
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

const char* pylt_obj_type_name(int ob_type) {
    return pylt_obj_basetypes[ob_type];
}
