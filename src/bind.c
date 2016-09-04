
#include "bind.h"
#include "intp.h"

void pylt_obj_type_register(PyLiteInterpreter *I, PyLiteTypeObject* type) {
    if (type->ob_reftype >= kv_max(I->cls_base)) {
        kv_resize(PyLiteTypeObject*, I->cls_base, type->ob_reftype + 10);
    }
    pylt_gc_add(I, castobj(type->name));
    kv_A(I->cls_base, type->ob_reftype) = type;
    I->cls_base.n = type->ob_reftype + 1;
}

PyLiteTupleObject* _NT(PyLiteInterpreter *I, int n, ...) {
    va_list args;
    PyLiteTupleObject *obj;

    va_start(args, n);
    obj = pylt_obj_tuple_new(I, n);

    for (int i = 0; i < n; ++i) {
        obj->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);
    return obj;
}

PyLiteTupleObject* _NST(PyLiteInterpreter *I, int n, ...) {
    va_list args;
    PyLiteTupleObject *obj;

    va_start(args, n);
    obj = pylt_obj_tuple_new(I, n);

    for (int i = 0; i < n; ++i) {
        obj->ob_val[i] = castobj(pylt_obj_str_new_from_cstr(I, va_arg(args, char*), true));
    }
    va_end(args);
    return obj;
}

pl_uint_t* _UINTS(PyLiteInterpreter *I, pl_uint_t n, ...) {
    va_list args;
    pl_uint_t *ret = pylt_malloc(I, sizeof(pl_uint_t) * n);

    va_start(args, n);
    for (pl_uint_t i = 0; i < n; ++i) {
        ret[i] = va_arg(args, pl_uint_t);
    }
    va_end(args);
    return ret;
}

void gc_register_cfunc(PyLiteInterpreter *I, PyLiteCFunctionObject *cfunc) {
	if (!cfunc) return;
	pylt_gc_add(I, castobj(cfunc));
	pylt_gc_add(I, castobj(cfunc->info.name));
	pylt_gc_add(I, castobj(cfunc->info.doc));
	pylt_gc_add(I, castobj(cfunc->info.params));
	pylt_gc_add(I, castobj(cfunc->info.defaults));
}

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(I, name, param_names, defaults, types, cfunc);
    pylt_obj_dict_setitem(I, mod->attrs, castobj(name), castobj(func));
    gc_register_cfunc(I, func);
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(I, name, param_names, defaults, types, cfunc);
    pylt_obj_type_setattr(I, type, castobj(name), castobj(func));
    gc_register_cfunc(I, func);
    return func;
}

PyLiteCFunctionObject* pylt_cmethod_register_0_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    // (..., _UINTS(1, type->ob_reftype), cfunc);
    return pylt_cmethod_register(I, type, name, _NST(I, 1, "self"), NULL, NULL, cfunc);
}

PyLiteCFunctionObject* pylt_cmethod_register_1_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(I, type, name, _NST(I, 2, "self", "object"), NULL, NULL, cfunc);
}

PyLiteCFunctionObject* pylt_cclsmethod_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_obj_cfunc_new(I, name, param_names, defaults, types, cfunc);
    pylt_obj_type_setattr(I, type, castobj(name), castobj(func));
    gc_register_cfunc(I, func);
    return func;
}

PyLiteCFunctionObject* pylt_cclsmethod_register_0_args(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_cmethod_register(I, type, name, _NST(I, 1, "cls"), NULL, NULL, cfunc);
}

void pylt_attr_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteObject *value) {
    pylt_obj_type_setattr(I, type, castobj(key), castobj(value));
    pylt_gc_add(I, castobj(key));
    pylt_gc_add(I, value);
}

void pylt_cprop_register(PyLiteInterpreter *I, PyLiteTypeObject *type, PyLiteStrObject *key, PyLiteCFunctionPtr cfget, PyLiteCFunctionPtr cfset) {
    PyLiteCFunctionObject *fget = cfget ? pylt_obj_cfunc_new(I, _S(fget), _NST(I, 1, "self"), NULL, NULL, cfget) : NULL;
    PyLiteCFunctionObject *fset = cfset ? pylt_obj_cfunc_new(I, _S(fset), _NST(I, 1, "self"), NULL, NULL, cfset) : NULL;
    PyLitePropertyObject *prop = pylt_obj_property_new(I, castobj(fget), castobj(fset));
    pylt_gc_add(I, castobj(key));
    gc_register_cfunc(I, fget);
    gc_register_cfunc(I, fset);
    pylt_attr_register(I, type, key, castobj(prop));
}
