
#ifndef PYLITE_TYPES_TUPLE_H
#define PYLITE_TYPES_TUPLE_H

#include "object.h"

typedef struct PyLiteTupleObject {
    PyLiteObject_HEAD;
    pl_int_t ob_size;
    PyLiteObject **ob_val;
} PyLiteTupleObject;

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteState *state, pl_int_t size);
PyLiteObject* pylt_obj_tuple_getitem(PyLiteState *state, PyLiteTupleObject *self, int index);
PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteState *state, pl_int_t len, void *data);
void pylt_obj_tuple_free(PyLiteState *state, PyLiteTupleObject *self);

#endif
