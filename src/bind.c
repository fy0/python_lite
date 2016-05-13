
#include "bind.h"
#include "state.h"

void pylt_obj_type_register(PyLiteState *state, PyLiteTypeObject* type) {
    if (type->ob_reftype >= kv_size(state->cls_base)) {
        kv_resize(PyLiteTypeObject*, state->cls_base, type->ob_reftype + 10);
    }
    kv_A(state->cls_base, type->ob_reftype) = type;
}

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
    for (pl_uint_t i = 0; i < n; ++i) {
        ret[i] = va_arg(args, pl_uint_t);
    }
    va_end(args);
    return ret;
}

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(state, name, param_names, defaults, types, cfunc);
    pylt_obj_dict_setitem(state, mod->attrs, castobj(name), castobj(func));
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(state, name, param_names, defaults, types, cfunc);
    pylt_obj_type_setattr(state, type, castobj(name), castobj(func));
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    // (..., _UINTS(1, type->ob_reftype), cfunc);
    return pylt_cmethod_register(state, type, name, _NST(state, 1, "self"), NULL, NULL, cfunc);
}

PyLiteCFunctionObject* pylt_cmethod_register_1_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(state, type, name, _NST(state, 2, "self", "object"), NULL, NULL, cfunc);
}

PyLiteCFunctionObject* pylt_cclsmethod_register_0_args(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(state, type, name, _NST(state, 1, "cls"), NULL, NULL, cfunc);
}

void pylt_attr_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteObject *value) {
    pylt_obj_type_setattr(state, type, castobj(key), castobj(value));
}

void pylt_cprop_register(PyLiteState *state, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteCFunctionPtr cfget, PyLiteCFunctionPtr cfset) {
    PyLiteCFunctionObject *fget = cfget ? pylt_obj_cfunc_new(state, key, _NST(state, 1, "self"), NULL, NULL, cfget) : NULL;
    PyLiteCFunctionObject *fset = cfset ? pylt_obj_cfunc_new(state, key, _NST(state, 1, "self"), NULL, NULL, cfset) : NULL;
    pylt_attr_register(state, type, key, castobj(pylt_obj_property_new(state, castobj(fget), castobj(fset))));
}
