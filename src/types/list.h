
#ifndef PYLITE_TYPES_LIST_H
#define PYLITE_TYPES_LIST_H

#include "object.h"

typedef struct PyLiteListObject {
    PyLiteObject_HEAD;
    pl_int_t ob_size;
    pl_int_t ob_maxsize;
    PyLiteObject **ob_val;
} PyLiteListObject;

PyLiteListObject* pylt_obj_list_new(PyLiteState *state);
PyLiteListObject* pylt_obj_list_new_with_size(PyLiteState *state, pl_uint_t size);
void pylt_obj_list_free(PyLiteState *state, PyLiteListObject *self);

pl_int_t pylt_obj_list_append(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj);
void pylt_obj_list_clear(PyLiteState *state, PyLiteListObject *self);
PyLiteListObject* pylt_obj_list_copy(PyLiteState *state, PyLiteListObject *self);
pl_uint_t pylt_obj_list_count(PyLiteState *state, PyLiteListObject *self);
void pylt_obj_list_extend(PyLiteState *state, PyLiteListObject *self, PyLiteListObject *obj);
pl_int_t pylt_obj_list_index(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj);
pl_int_t pylt_obj_list_index_strict(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj);
pl_bool_t pylt_obj_list_insert(PyLiteState *state, PyLiteListObject *self, pl_int_t index, PyLiteObject *obj);
PyLiteObject* pylt_obj_list_pop(PyLiteState *state, PyLiteListObject *self);
pl_bool_t pylt_obj_list_remove(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj);
void pylt_obj_list_reverse(PyLiteState *state, PyLiteListObject *self);
void pylt_obj_list_sort(PyLiteState *state, PyLiteListObject *self);

PyLiteObject* pylt_obj_list_getitem(PyLiteState *state, PyLiteListObject *self, int index);
pl_bool_t pylt_obj_list_setitem(PyLiteState *state, PyLiteListObject *self, int index, PyLiteObject* obj);

pl_bool_t pylt_obj_list_has(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj);

#endif
