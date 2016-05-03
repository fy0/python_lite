
#ifndef PYLITE_BIND_BIND_H
#define PYLITE_BIND_BIND_H

#include "types/all.h"

void pylt_obj_type_register(PyLiteState *state, PyLiteTypeObject* type);

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register_1_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cclsmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);
void pylt_cprop_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteCFunctionPtr cfget, PyLiteCFunctionPtr cfset);
void pylt_attr_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteObject *value);

#define _NS(state, str) pylt_obj_str_new_from_c_str(state, str, true)

PyLiteTupleObject* _NT(PyLiteState *state, int n, ...);
PyLiteTupleObject* _NST(PyLiteState *state, int n, ...);

pl_uint_t* _UINTS(pl_uint_t n, ...);

//#define NS(str) pylt_obj_str_new_from_c_str(state, str, true)

#define PARAM_NODEF  0
#define PARAM_ARGS   1
#define PARAM_KWARGS 2

#endif
