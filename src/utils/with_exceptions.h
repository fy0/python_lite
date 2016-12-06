
#ifndef PYLITE_WITH_EXCEPTIONS_H
#define PYLITE_WITH_EXCEPTIONS_H

#include "../types/all.h"

// object api with exceptions

PyLiteBytesObject* pylt_obj_bytes_Egetitem(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *index);
PyLiteStrObject* pylt_obj_str_Egetitem(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_tuple_Egetitem(PyLiteInterpreter *I, PyLiteTupleObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_list_Egetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_dict_Egetitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key);
pl_bool_t pylt_obj_list_Esetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index, PyLiteObject* obj);

pl_bool_t pylt_obj_dict_Eremove(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key);

PyLiteObject* pylt_obj_Egetitem(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key);
pl_bool_t pylt_obj_Esetitem(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);
pl_bool_t pylt_obj_Edelitem(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key);

PyLiteObject* pylt_obj_Egetattr_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, PyLiteObject* _default, pl_bool_t *p_at_type);
PyLiteObject* pylt_obj_Egetattr(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type);
pl_bool_t pylt_obj_Esetattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);
pl_bool_t pylt_obj_Edelattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key);

pl_int_t pylt_obj_Ecmp(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b);

PyLiteObject* pylt_obj_Eslice_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject *start, PyLiteObject *end, PyLiteObject *step);
void pylt_obj_Eslice_set_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject *start, PyLiteObject *end, PyLiteObject *step, PyLiteObject *val);


PyLiteStrObject* pylt_obj_Eto_str(PyLiteInterpreter *I, PyLiteObject *obj);
PyLiteStrObject* pylt_obj_Eto_repr(PyLiteInterpreter *I, PyLiteObject *obj);

PyLiteIterObject* pylt_obj_iter_Enew(PyLiteInterpreter *I, PyLiteObject *obj);
PyLiteObject* pylt_obj_iter_Enext(PyLiteInterpreter *I, PyLiteIterObject *iter);

#endif
