﻿
#include "helper.h"
#include "../state.h"
#include "../debug.h"
#include "../types/function.h"

PyLiteTupleObject* _NT(PyLiteState *state, int n, ...) {
    va_list args;
    PyLiteTupleObject *obj;

    va_start(args, n);
    obj = pylt_obj_tuple_new(state, n);

    for (int i = 0; i < n; ++i) {
        obj->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);
    return obj;
}

PyLiteTupleObject* _NST(PyLiteState *state, int n, ...) {
    va_list args;
    PyLiteTupleObject *obj;

    va_start(args, n);
    obj = pylt_obj_tuple_new(state, n);

    for (int i = 0; i < n; ++i) {
        obj->ob_val[i] = castobj(pylt_obj_str_new_from_c_str(state, va_arg(args, char*), true));
    }
    va_end(args);
    return obj;
}

pl_uint_t* _UINTS(pl_uint_t n, ...) {
    va_list args;
    pl_uint_t *ret = pylt_realloc(NULL, sizeof(pl_uint_t) * n);

    va_start(args, n);
    for (int i = 0; i < n; ++i) {
        ret[i] = va_arg(args, pl_uint_t);
    }
    va_end(args);
    return ret;
}

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(state, name, param_names, defaults, types, cfunc);
    pylt_obj_table_set(mod->attrs, castobj(name), castobj(func));
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(state, name, param_names, defaults, types, cfunc);
    pylt_obj_type_setattr(state, type, castobj(name), castobj(func));
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(state, type, name, _NST(state, 1, "self"), NULL, _UINTS(1, type->ob_reftype), cfunc);
}

PyLiteCFunctionObject* pylt_cmethod_register_1_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(state, type, name, _NST(state, 2, "self", "object"), NULL, _UINTS(2, type->ob_reftype, 0), cfunc);
}
