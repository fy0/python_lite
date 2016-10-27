﻿
#ifndef PYLITE_API_H
#define PYLITE_API_H

#include "config.h"
#include "types/all.h"

void pylt_api_output_str(PyLiteInterpreter *I, PyLiteStrObject *obj);

PyLiteStrObject* pylt_api_type_name(PyLiteInterpreter *I, int ob_type);

PyLiteTypeObject* pylt_api_getbuiltintype(PyLiteInterpreter *I, PyLiteStrObject *name);
PyLiteTypeObject* pylt_api_gettype_from_mod(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name);
PyLiteTypeObject* pylt_api_gettype_by_code(PyLiteInterpreter *I, pl_uint32_t type_code);
// get first builtin parent type (type of ob_base for custom type)
pl_uint32_t pylt_api_get_base_typecode(PyLiteInterpreter *I, pl_uint32_t type_code);
pl_bool_t pylt_api_isinstance(PyLiteInterpreter *I, PyLiteObject *obj, pl_uint32_t type_code);

// return true if a is subclass of b
pl_bool_t pl_api_issubclass(PyLiteInterpreter *I, PyLiteTypeObject *a, PyLiteTypeObject *b);

PyLiteTypeObject* pl_type(PyLiteInterpreter *I, PyLiteObject *obj);
void pl_print(PyLiteInterpreter *I, const char *format, ...);
void pl_error(PyLiteInterpreter *I, PyLiteStrObject *exception_name, const char *format, ...);
// pl_error(PyLiteInterpreter *I, ...)

#endif
