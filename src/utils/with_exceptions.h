
#ifndef PYLITE_WITH_EXCEPTIONS_H
#define PYLITE_WITH_EXCEPTIONS_H

#include "../types/all.h"

// api with exceptions
PyLiteBytesObject* pylt_obj_bytes_Egetitem(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *index);
PyLiteStrObject* pylt_obj_str_Egetitem(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_tuple_Egetitem(PyLiteInterpreter *I, PyLiteTupleObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_list_Egetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index);
PyLiteObject* pylt_obj_dict_Egetitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key);

pl_bool_t pylt_obj_list_Esetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index, PyLiteObject* obj);

#endif
