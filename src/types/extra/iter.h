
#ifndef PYLITE_TYPES_ITER_H
#define PYLITE_TYPES_ITER_H

#include "../object.h"

// Common Iterator
typedef struct PyLiteIterObject {
    PyLiteObject_HEAD;
    PyLiteObject *base;
    PyLiteIterFunc iter_func;
    union {
        struct {
            pl_int_t count;
            pl_int_t index;
        } array;
        struct {
            pl_int_t count;
            pl_uint_t k;
        } hashmap;
    };
} PyLiteIterObject;

PyLiteIterObject* pylt_obj_iter_new(PyLiteState *state, PyLiteObject *obj);
PyLiteObject* pylt_obj_iter_next(PyLiteState *state, PyLiteIterObject *iter);

PyLiteObject* pylt_obj_bytes_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_str_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_tuple_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_list_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_set_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_dict_iternext(PyLiteState *state, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_range_iternext(PyLiteState *state, PyLiteIterObject *iter);

void pylt_obj_iter_free(PyLiteState *state, PyLiteIterObject* self);

#endif
