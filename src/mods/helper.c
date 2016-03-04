
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

PyLiteCFunction* pylt_func_bind(PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, PyLiteTupleObject *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunction *func = pylt_realloc(NULL, sizeof(PyLiteCFunction));
    func->length = param_names ? param_names->ob_size : 0;
    func->minimal = func->length - (defaults ? defaults->ob_size : 0);
    func->names = param_names ? param_names->ob_val : NULL;
    func->defaults = defaults ? defaults->ob_val : NULL;
    func->func = cfunc;

    pylt_free(param_names);
    pylt_free(defaults);
    return func;
}
