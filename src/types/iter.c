﻿
#include "iter.h"
#include "all.h"

PyLiteIterObject* pylt_obj_iter_new(PyLiteState *state, PyLiteObject *obj) {
    PyLiteIterObject *iter = pylt_realloc(NULL, sizeof(PyLiteIterObject));
    iter->ob_type = PYLT_OBJ_TYPE_ITER;
    iter->base = obj;

    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_bytes_iternext;
            return iter;
        case PYLT_OBJ_TYPE_STR:
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_str_iternext;
            return iter;
        case PYLT_OBJ_TYPE_TUPLE:
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_tuple_iternext;
            return iter;
        case PYLT_OBJ_TYPE_LIST:
            iter->array.index = 0;
            return iter;
        case PYLT_OBJ_TYPE_SET:
            iter->hashmap.count = pylt_obj_set_len(state, castset(obj));
            iter->hashmap.k = pylt_obj_set_begin(state, castset(obj));
            iter->iter_func = &pylt_obj_set_iternext;
            return iter;
        case PYLT_OBJ_TYPE_DICT:
            break;
    }
    return NULL;
}

PyLiteObject* pylt_obj_iter_next(PyLiteState *state, PyLiteIterObject *iter) {
    return (*iter->iter_func)(state, iter);
}

PyLiteObject* pylt_obj_bytes_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    char buf[1];
    int len = castbytes(iter->base)->ob_size;
    if (iter->array.index == len) return NULL;
    else {
        buf[0] = castbytes(iter->base)->ob_val[iter->array.index++];
        return castobj(pylt_obj_bytes_new(state, buf, 1, true));
    }
    return NULL;
}

PyLiteObject* pylt_obj_str_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    uint32_t buf[1];
    int len = caststr(iter->base)->ob_size;
    if (iter->array.index == len) return NULL;
    else {
        buf[0] = caststr(iter->base)->ob_val[iter->array.index++];
        return castobj(pylt_obj_str_new(state, buf, 1, true));
    }
    return NULL;
}

PyLiteObject* pylt_obj_tuple_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    int len = casttuple(iter->base)->ob_size;
    if (iter->array.index < len) {
        return casttuple(iter->base)->ob_val[iter->array.index++];
    }
    return NULL;
}

PyLiteObject* pylt_obj_list_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    return NULL;
}

PyLiteObject* pylt_obj_set_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    pl_uint_t k = iter->hashmap.k;
    if (pylt_obj_set_len(state, castset(iter->base)) != iter->hashmap.count)
        return NULL;

    k = pylt_obj_set_next(state, castset(iter->base), k);
    iter->hashmap.k = k;
    return pylt_obj_set_itemvalue(state, castset(iter->base), k);
}

PyLiteObject* pylt_obj_dict_iternext(PyLiteState *state, PyLiteIterObject *iter) {
    return NULL;
}