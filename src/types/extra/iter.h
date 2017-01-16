
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
            pl_int32_t k;
        } hashmap;
    };
    struct PyLiteIterObject *backup; // isn't tracked by gc
} PyLiteIterObject;

PyLiteIterObject* pylt_obj_iter_new(PyLiteInterpreter *I, PyLiteObject *obj);
PyLiteObject* pylt_obj_iter_next(PyLiteInterpreter *I, PyLiteIterObject *iter);

PyLiteObject* pylt_obj_bytes_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_str_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_tuple_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_list_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_set_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_dict_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_dict_items_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_range_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);
PyLiteObject* pylt_obj_custom_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter);

void pylt_obj_iter_rfree(PyLiteInterpreter *I, PyLiteIterObject* self);
void pylt_obj_iter_free(PyLiteInterpreter *I, PyLiteIterObject* self);

#endif
