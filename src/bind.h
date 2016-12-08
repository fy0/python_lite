
#ifndef PYLITE_BIND_H
#define PYLITE_BIND_H

#include "utils/config.h"
#include "types/all.h"

pl_bool_t pl_bind_cls_check(PyLiteInterpreter *I, PyLiteTypeObject *defclass, PyLiteStrObject *method_name, PyLiteTypeObject *givenclass);
PyLiteObject* pylt_obj_typecast(PyLiteInterpreter *I, struct PyLiteTypeObject *type, PyLiteObject *obj);
void pylt_obj_type_register(PyLiteInterpreter *I, PyLiteTypeObject *type);

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register_1_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cclsmethod_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cclsmethod_register_0_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
void pylt_cprop_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteCFunctionPtr cfget, PyLiteCFunctionPtr cfset);
void pylt_attr_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteObject *value);

#define _NS(I, str) pylt_obj_str_new_from_cstr(I, str, true)
#define _NI(_int_val) pylt_obj_int_new(I, (_int_val))

// static string
#define _S(name) pl_static.str.name

PyLiteTupleObject* _NT(PyLiteInterpreter *I, int n, ...);
PyLiteTupleObject* _NST(PyLiteInterpreter *I, int n, ...);

pl_uint_t* _UINTS(PyLiteInterpreter *I, pl_uint_t n, ...);

#endif
