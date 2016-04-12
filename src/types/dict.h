
#ifndef PYLITE_TYPES_DICT_H
#define PYLITE_TYPES_DICT_H

#include "object.h"

typedef struct PyLiteDictObject {
    PyLiteObject_HEAD;
    PyLiteTable *ob_val;
} PyLiteDictObject;

pl_int_t pylt_obj_dict_ccmp(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_dict_ceq(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other);


PyLiteObject* pylt_obj_dict_cgetitem(PyLiteState *state, PyLiteDictObject *self, int index);
pl_bool_t pylt_obj_dict_csetitem(PyLiteState *state, PyLiteDictObject *self, int index, PyLiteObject* obj);

pl_int_t pylt_obj_dict_len(PyLiteState *state, PyLiteDictObject *self);
PyLiteObject* pylt_obj_dict_has(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *obj);

PyLiteObject* pylt_obj_dict_pop(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *obj);

pl_int_t pylt_obj_dict_begin(PyLiteState *state, PyLiteDictObject *self);
pl_int_t pylt_obj_dict_end(PyLiteState *state, PyLiteDictObject *self);
void pylt_obj_dict_next(PyLiteState *state, PyLiteDictObject *self, pl_int_t *k);
PyLiteObject* pylt_obj_dict_itemkey(PyLiteState *state, PyLiteDictObject *self, pl_int_t k);
PyLiteObject* pylt_obj_dict_itemvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k);
void pylt_obj_dict_keyvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k, PyLiteObject **key, PyLiteObject **val);

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state);

#endif
