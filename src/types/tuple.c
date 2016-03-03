﻿
#include "tuple.h"

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteState *state, pl_int_t len) {
    PyLiteTupleObject *obj = pylt_realloc(NULL, sizeof(PyLiteTupleObject));
    obj->ob_type = PYLT_OBJ_TYPE_TUPLE;
    obj->ob_val = pylt_realloc(NULL, len * sizeof(PyLiteObject*));
    return obj;
}


PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteState *state, pl_int_t len, PyLiteObject *objs) {
    PyLiteTupleObject *obj = pylt_obj_tuple_new(state, len);
    memcpy(obj->ob_val, objs, len * sizeof(PyLiteObject*));
    return obj;
}
