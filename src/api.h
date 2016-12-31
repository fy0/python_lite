
#ifndef PYLITE_API_H
#define PYLITE_API_H

#include "utils/config.h"
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
PyLiteTypeObject* pl_type_by_code(PyLiteInterpreter *I, pl_uint32_t type_code);

void pl_print(PyLiteInterpreter *I, const char *format, ...);
void pl_error(PyLiteInterpreter *I, PyLiteStrObject *exception_name, const char *format, ...);

PyLiteObject* pl_call(PyLiteInterpreter *I, PyLiteObject *callable, int argc, ...);
PyLiteObject* pl_call_method(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, int argc, ...);
PyLiteObject* pl_call_method_ex(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, PyLiteTupleObject *args, PyLiteDictObject *kwargs);

#define pl_new pl_call
#define pl_format pylt_obj_str_new_from_cformat

#define pl_assert(I, stmt, ret) if (!(stmt)) { pl_error(I, pl_static.str.RuntimeError, "panic"); return (ret); }

#endif
