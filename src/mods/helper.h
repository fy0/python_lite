﻿
#ifndef PYLITE_MODS_HELPER_H
#define PYLITE_MODS_HELPER_H

#include "../types/all.h"
#include "mods.h"

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);


#define _NS(state, str) pylt_obj_str_new_from_c_str(state, str, true)

PyLiteTupleObject* _NT(PyLiteState *state, int n, ...);
PyLiteTupleObject* _NST(PyLiteState *state, int n, ...);

int* _INTS(int n, ...);

//#define NS(str) pylt_obj_str_new_from_c_str(state, str, true)

#define pylt_mod_setattr(state, mod, key_str, value) pylt_obj_table_set((mod)->attrs, castobj(_NS(state, (key_str))), castobj(value));

#define PARAM_NODEF  0
#define PARAM_ARGS   1
#define PARAM_KWARGS 2

#endif
