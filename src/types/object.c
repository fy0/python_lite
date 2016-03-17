
#include "../vm.h"
#include "object.h"
#include "number.h"
#include "bytes.h"
#include "bool.h"
#include "string.h"
#include "bytes.h"
#include "set.h"


void* basetype_op_func_table[][23] = {
    /* { //LT, LE, GT, GE, NE, EQ, 
     BITOR, BITXOR, BITAND, BITLS, BITRS, 
     PLUS, MINUS, MUL, MATMUL, DIV, FLOORDIV, MOD, 
     POS, NEG, BITNOT,  POW }
    */
    { // object
        //NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // int
        //&pylt_obj_int_lt, &pylt_obj_int_le, &pylt_obj_int_gt, &pylt_obj_int_ge, &pylt_obj_int_ne, &pylt_obj_int_eq,
        &pylt_obj_int_bitor, &pylt_obj_int_bitxor, &pylt_obj_int_bitand, &pylt_obj_int_bitls, &pylt_obj_int_bitrs,
        &pylt_obj_int_plus, &pylt_obj_int_minus, &pylt_obj_int_mul, NULL, &pylt_obj_int_div, &pylt_obj_int_floordiv, &pylt_obj_int_mod,
        &pylt_obj_int_positive, &pylt_obj_int_negative, &pylt_obj_int_bitnot, &pylt_obj_int_pow
    },
    { // float
        //&pylt_obj_float_lt, &pylt_obj_float_le, &pylt_obj_float_gt, &pylt_obj_float_ge, &pylt_obj_float_ne, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, &pylt_obj_float_minus, &pylt_obj_float_mul, NULL, &pylt_obj_float_div, &pylt_obj_float_floordiv, &pylt_obj_float_mod,
        &pylt_obj_float_positive, &pylt_obj_float_negative, NULL, &pylt_obj_float_pow
    },
    { // bool
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_bool_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },

    { // str
        //NULL, NULL, NULL, NULL, &pylt_obj_str_ne, &pylt_obj_str_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_str_plus, NULL, &pylt_obj_str_mul, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // bytes
        //NULL, NULL, NULL, NULL, &pylt_obj_bytes_ne, &pylt_obj_bytes_eq,
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

    { // function
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

    { // type
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
        NULL, NULL, NULL, NULL, NULL,
        &pylt_obj_float_plus, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    },
    { // class
        //NULL, NULL, NULL, NULL, NULL, &pylt_obj_float_eq,
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

pl_int_t pylt_obj_ccmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return 0;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_ccmp(state, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_ccmp(state, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_ccmp(state, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_ccmp(state, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_ccmp(state, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_ccmp(state, castset(a), b);
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
    }
    // custom type
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
            if (obj->ob_type >= PYLT_OBJ_TYPE_CLASS) {
                //pylt_obj_table_get(castclass(obj)->ob_attrs, "__hash__");
                return false;
            }
    }
    return false;
}


pl_bool_t pylt_obj_citerable(PyLiteState *state, PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
        case PYLT_OBJ_TYPE_STR:
        case PYLT_OBJ_TYPE_TUPLE:
        case PYLT_OBJ_TYPE_SET:
        case PYLT_OBJ_TYPE_DICT:
            return true;
        default:
            if (obj->ob_type > PYLT_OBJ_TYPE_NUM) {
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
        case PYLT_OBJ_TYPE_STR: return castbytes(obj)->ob_size != 0;
        default: return true;
    }
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
        case OP_IS: return NULL; // TODO
        case OP_IS_NOT: return NULL; // TODO
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

    "module",
    "function",
    "cfunction",

    "type",
    "iter",
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
