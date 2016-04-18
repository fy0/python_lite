
#include "helper.h"
#include "../state.h"
#include "../debug.h"

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

int* _INTS(int n, ...) {
    va_list args;
    int *ret = pylt_realloc(NULL, sizeof(int) * n);

    va_start(args, n);
    for (int i = 0; i < n; ++i) {
        ret[i] = va_arg(args, int);
    }
    va_end(args);
    return ret;
}

PyLiteCFunctionObject* pylt_cfunc_register(PyLiteModuleObject *mod, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_realloc(NULL, sizeof(PyLiteCFunctionObject));
    func->ob_type = PYLT_OBJ_TYPE_CFUNCTION;
    func->info.length = param_names ? param_names->ob_size : 0;
    func->info.minimal = func->info.length - (defaults ? defaults->ob_size : 0);
    func->info.names = param_names ? (PyLiteStrObject**)param_names->ob_val : NULL;
    func->info.defaults = defaults ? defaults->ob_val : NULL;
    func->info.type_codes = types;
    func->code = cfunc;
    pylt_obj_table_set(mod->attrs, castobj(name), castobj(func));

    pylt_free(param_names);
    pylt_free(defaults);
    return func;
}
