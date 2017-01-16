
#ifndef PYLITE_TYPES_SET_H
#define PYLITE_TYPES_SET_H

#include "../object.h"

KHASHO_INIT(set_obj, PyLiteObject*, char, 0, pylt_obj_hash, pylt_obj_eq);

typedef struct PyLiteSetObject {
    PyLiteObject_HEAD;
    khasho_t(set_obj) *ob_val;
} PyLiteSetObject;

pl_int_t pylt_obj_set_cmp(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_set_eq(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteObject *other);

pl_int_t pylt_obj_set_len(PyLiteInterpreter *I, PyLiteSetObject *self);
pl_int_t pylt_obj_set_add(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteObject *obj);
void pylt_obj_set_clear(PyLiteInterpreter *I, PyLiteSetObject *self);
PyLiteSetObject* pylt_obj_set_copy(PyLiteInterpreter *I, PyLiteSetObject *self);
pl_bool_t pylt_obj_set_contains(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteSetObject *other);
PyLiteObject* pylt_obj_set_has(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteObject *obj);

pl_int_t pylt_obj_set_remove(PyLiteInterpreter *I, PyLiteSetObject *self, PyLiteObject *obj);
PyLiteObject* pylt_obj_set_pop(PyLiteInterpreter *I, PyLiteSetObject *self);

pl_int32_t pylt_obj_set_begin(PyLiteInterpreter *I, PyLiteSetObject *self);
pl_int32_t pylt_obj_set_end(PyLiteInterpreter *I, PyLiteSetObject *self);
void pylt_obj_set_next(PyLiteInterpreter *I, PyLiteSetObject *self, pl_int32_t *k);

// 存在则取值 否则为空
PyLiteObject* pylt_obj_set_itemvalue(PyLiteInterpreter *I, PyLiteSetObject *self, pl_int32_t k);

struct PyLiteStrObject* pylt_obj_set_to_str(PyLiteInterpreter *I, PyLiteSetObject *self);

PyLiteSetObject* pylt_obj_set_new(PyLiteInterpreter *I);
void pylt_obj_set_rfree(PyLiteInterpreter *I, PyLiteSetObject* self);
void pylt_obj_set_free(PyLiteInterpreter *I, PyLiteSetObject* self);

#endif
