
#ifndef PYLITE_MODS_HELPER_H
#define PYLITE_MODS_HELPER_H

#include "../types/all.h"
#include "mods.h"

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc);

#define _NS(state, str) pylt_obj_str_new_from_c_str(state, str, true)

PyLiteTupleObject* _NT(PyLiteState *state, int n, ...);
PyLiteTupleObject* _NST(PyLiteState *state, int n, ...);

int* _INTS(int n, ...);

//#define NS(str) pylt_obj_str_new_from_c_str(state, str, true)

#define PARAM_NODEF  0
#define PARAM_ARGS   1
#define PARAM_KWARGS 2

#endif
