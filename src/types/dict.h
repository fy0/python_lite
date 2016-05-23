
#ifndef PYLITE_TYPES_DICT_H
#define PYLITE_TYPES_DICT_H

#include "object.h"

KHASHO_INIT(table, PyLiteObject*, PyLiteObject*, 1, pylt_obj_hash, pylt_obj_eq);

typedef struct PyLiteDictObject {
    PyLiteObject_HEAD;
    khasho_t(table) *ob_val;
} PyLiteDictObject;


pl_int_t pylt_obj_dict_cmp(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_dict_eq(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other);


PyLiteObject* pylt_obj_dict_getitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key);
void pylt_obj_dict_setitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject* key, PyLiteObject* value);

pl_int_t pylt_obj_dict_len(PyLiteState *state, PyLiteDictObject *self);
PyLiteDictObject* pylt_obj_dict_copy(PyLiteState *state, PyLiteDictObject *self);
pl_bool_t pylt_obj_dict_has(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *obj);
pl_bool_t pylt_obj_dict_remove(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key);
PyLiteObject* pylt_obj_dict_pop(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *obj);

pl_int_t pylt_obj_dict_begin(PyLiteState *state, PyLiteDictObject *self);
pl_int_t pylt_obj_dict_end(PyLiteState *state, PyLiteDictObject *self);
void pylt_obj_dict_next(PyLiteState *state, PyLiteDictObject *self, pl_int_t *k);

PyLiteObject* pylt_obj_dict_itemkey(PyLiteState *state, PyLiteDictObject *self, pl_int_t k);
PyLiteObject* pylt_obj_dict_itemvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k);
void pylt_obj_dict_keyvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k, PyLiteObject **key, PyLiteObject **val);

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state);
void pylt_obj_dict_free(PyLiteState *state, PyLiteDictObject *self);

#endif
