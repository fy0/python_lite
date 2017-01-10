
#ifndef PYLITE_TYPES_DICT_H
#define PYLITE_TYPES_DICT_H

#include "../object.h"

KHASHO_INIT(table, PyLiteObject*, PyLiteObject*, 1, pylt_obj_hash, pylt_obj_eq);

typedef struct PyLiteDictObject {
    PyLiteObject_HEAD;
    khasho_t(table) *ob_val;
} PyLiteDictObject;


pl_int_t pylt_obj_dict_cmp(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_dict_eq(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *other);


PyLiteObject* pylt_obj_dict_getitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key);
void pylt_obj_dict_setitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject* key, PyLiteObject* value);

pl_int_t pylt_obj_dict_len(PyLiteInterpreter *I, PyLiteDictObject *self);
PyLiteDictObject* pylt_obj_dict_copy(PyLiteInterpreter *I, PyLiteDictObject *self);
pl_bool_t pylt_obj_dict_has(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *obj);
pl_bool_t pylt_obj_dict_remove(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key);
PyLiteObject* pylt_obj_dict_pop(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *obj);
void pylt_obj_dict_update(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteDictObject *other);

pl_int32_t pylt_obj_dict_begin(PyLiteInterpreter *I, PyLiteDictObject *self);
pl_int32_t pylt_obj_dict_end(PyLiteInterpreter *I, PyLiteDictObject *self);
void pylt_obj_dict_next(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t *k);

PyLiteObject* pylt_obj_dict_itemkey(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k);
PyLiteObject* pylt_obj_dict_itemvalue(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k);
void pylt_obj_dict_keyvalue(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k, PyLiteObject **key, PyLiteObject **val);
struct PyLiteStrObject* pylt_obj_dict_to_str(PyLiteInterpreter *I, PyLiteDictObject *self);

PyLiteDictObject* pylt_obj_dict_new(PyLiteInterpreter *I);
void pylt_obj_dict_free(PyLiteInterpreter *I, PyLiteDictObject *self);

#endif
