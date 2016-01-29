
#include "object.h"
#include "number.h"
#include "bytes.h"
#include "bool.h"

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

uint32_t pylt_obj_eq(PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return true;
    switch (a->ob_type) {
    case PYLT_OBJ_TYPE_INT: return pylt_obj_int_eq(castint(a), b);
    case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_eq(castfloat(a), b);
    case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_eq(castbool(a), b);
    case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_eq(castbytes(a), b);
    }
    return false;
}

void pylt_obj_free(PyLiteObject *obj) {
    ;
}

bool pylt_obj_hashable(PyLiteObject *obj) {
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
