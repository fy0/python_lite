
#include "function.h"

PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state, PyLiteCodeSnippetObject *code) {
    PyLiteFunctionObject *obj = pylt_realloc(NULL, sizeof(PyLiteFunctionObject));
    memset(obj, 0, sizeof(PyLiteFunctionObject));
    obj->ob_type = PYLT_OBJ_TYPE_FUNCTION;
    if (code) {
        memcpy(&obj->code, code, sizeof(PyLiteCodeSnippetObject));
    } else {
        kv_init(obj->code.const_val);
        kv_init(obj->code.opcodes);
    }
    return obj;
}

PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteState *state, PyLiteObject *func) {
    if (func->ob_type == PYLT_OBJ_TYPE_FUNCTION)
        return &castfunc(func)->info;
    if (func->ob_type == PYLT_OBJ_TYPE_CFUNCTION)
        return &castcfunc(func)->info;
    return NULL;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new(PyLiteState *state, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_realloc(NULL, sizeof(PyLiteCFunctionObject));
    func->ob_type = PYLT_OBJ_TYPE_CFUNCTION;
    func->info.length = param_names ? param_names->ob_size : 0;
    func->info.minimal = func->info.length - (defaults ? defaults->ob_size : 0);

    func->info.name = name;
    func->info.params = param_names ? (PyLiteStrObject**)param_names->ob_val : NULL;
    func->info.defaults = defaults ? defaults->ob_val : NULL;
    func->info.type_codes = types;
    func->code = cfunc;

    pylt_free(param_names);
    pylt_free(defaults);
    return func;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteState *state, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_obj_cfunc_new(state, name, NULL, NULL, NULL, cfunc);
}
