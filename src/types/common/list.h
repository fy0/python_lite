
#ifndef PYLITE_TYPES_LIST_H
#define PYLITE_TYPES_LIST_H

#include "../object.h"

typedef struct PyLiteListObject {
    PyLiteObject_HEAD;
    pl_int_t ob_size;
    pl_int_t ob_maxsize;
    PyLiteObject **ob_val;
} PyLiteListObject;

pl_bool_t pylt_obj_list_eq(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *other);

pl_int_t pylt_obj_list_append(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj);
void pylt_obj_list_clear(PyLiteInterpreter *I, PyLiteListObject *self);
PyLiteListObject* pylt_obj_list_copy(PyLiteInterpreter *I, PyLiteListObject *self);
pl_uint_t pylt_obj_list_count(PyLiteInterpreter *I, PyLiteListObject *self);
void pylt_obj_list_extend(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteListObject *obj);
pl_int_t pylt_obj_list_index(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj);
pl_int_t pylt_obj_list_index_strict(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj);
pl_bool_t pylt_obj_list_insert(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t index, PyLiteObject *obj);
PyLiteObject* pylt_obj_list_pop(PyLiteInterpreter *I, PyLiteListObject *self);
pl_bool_t pylt_obj_list_remove(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj);
void pylt_obj_list_reverse(PyLiteInterpreter *I, PyLiteListObject *self);
void pylt_obj_list_sort(PyLiteInterpreter *I, PyLiteListObject *self);

PyLiteObject* pylt_obj_list_getitem(PyLiteInterpreter *I, PyLiteListObject *self, int index);
pl_bool_t pylt_obj_list_setitem(PyLiteInterpreter *I, PyLiteListObject *self, int index, PyLiteObject* obj);
pl_bool_t pylt_obj_list_delitem(PyLiteInterpreter *I, PyLiteListObject *self, int index);
PyLiteListObject* pylt_obj_list_slice(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step);
pl_int_t pylt_obj_list_slice_set(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step, PyLiteObject *val);
void pylt_obj_list_slice_del(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step, PyLiteObject *val);

pl_bool_t pylt_obj_list_has(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj);
struct PyLiteStrObject* pylt_obj_list_to_str(PyLiteInterpreter *I, PyLiteListObject *self);

PyLiteListObject* pylt_obj_list_new(PyLiteInterpreter *I);
PyLiteListObject* pylt_obj_list_new_with_size(PyLiteInterpreter *I, pl_uint_t size);
void pylt_obj_list_free(PyLiteInterpreter *I, PyLiteListObject *self);

#endif
