
#include "tuple.h"

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteState *state, pl_int_t len) {
    PyLiteTupleObject *obj = pylt_realloc(NULL, sizeof(PyLiteTupleObject));
    obj->ob_type = PYLT_OBJ_TYPE_TUPLE;
    obj->ob_val = (len) ? pylt_realloc(NULL, len * sizeof(PyLiteObject*)) : NULL;
    obj->ob_size = len;
    return obj;
}


PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteState *state, pl_int_t len, void *data) {
    PyLiteTupleObject *obj = pylt_obj_tuple_new(state, len);
    if (len) memcpy(obj->ob_val, data, len * sizeof(PyLiteObject*));
    return obj;
}

PyLiteObject* pylt_obj_tuple_getitem(PyLiteState *state, PyLiteTupleObject *self, int index) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return NULL;
    return self->ob_val[index];
}

void pylt_obj_tuple_free(PyLiteState *state, PyLiteTupleObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}
