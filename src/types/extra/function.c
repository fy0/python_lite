
#include "function.h"
#include "../../bind.h"

PyLiteFunctionObject* pylt_obj_func_new(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteFunctionObject *obj = pylt_realloc(NULL, sizeof(PyLiteFunctionObject));
    memset(obj, 0, sizeof(PyLiteFunctionObject));
    obj->ob_type = PYLT_OBJ_TYPE_FUNCTION;
    if (code) {
        memcpy(&obj->code, code, sizeof(PyLiteCodeObject));
    } else {
        obj->code.const_val = pylt_obj_list_new(I);
        kv_init(obj->code.opcodes);
    }
    return obj;
}

PyLiteFunctionObject* pylt_obj_func_new_ex(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteListObject *params, PyLiteCodeObject *code, PyLiteDictObject *defaults, PyLiteStrObject *args, PyLiteStrObject *kwargs) {
    PyLiteFunctionObject *func = pylt_obj_func_new(I, code);

    // set name and params
    func->info.name = name;
    func->info.length = params->ob_size;
    func->info.params = pylt_realloc(NULL, params->ob_size * sizeof(PyLiteObject*));
    memcpy(func->info.params, params->ob_val, params->ob_size * sizeof(PyLiteStrObject*));

    // set default values
    if (defaults || args || kwargs) {
        pl_int_t minimal = -1;
        PyLiteObject **defvals = pylt_realloc(NULL, params->ob_size * sizeof(PyLiteObject*));

        for (int i = 0; i < params->ob_size; ++i) {
            PyLiteStrObject *name = caststr(params->ob_val[i]);
            if (name == args) {
                if (minimal == -1) minimal = i;
                defvals[i] = castobj(PARAM_ARGS);
            } else if (name == kwargs) {
                if (minimal == -1) minimal = i;
                defvals[i] = castobj(PARAM_KWARGS);
            } else if (pylt_obj_dict_has(I, defaults, castobj(name))) {
                if (minimal == -1) minimal = i;
                defvals[i] = pylt_obj_dict_getitem(I, defaults, castobj(name));
            } else defvals[i] = NULL;
        }

        func->info.defaults = defvals;
        func->info.minimal = minimal;
    } else {
        func->info.defaults = NULL;
        func->info.minimal = params->ob_size;
    }
    return func;
}

PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteInterpreter *I, PyLiteObject *func) {
    if (func->ob_type == PYLT_OBJ_TYPE_FUNCTION)
        return &castfunc(func)->info;
    if (func->ob_type == PYLT_OBJ_TYPE_CFUNCTION)
        return &castcfunc(func)->info;
    return NULL;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc) {
    PyLiteCFunctionObject *func = pylt_realloc(NULL, sizeof(PyLiteCFunctionObject));
    func->ob_type = PYLT_OBJ_TYPE_CFUNCTION;
    func->info.length = param_names ? param_names->ob_size : 0;
    func->info.minimal = func->info.length - (defaults ? defaults->ob_size : 0);

    func->info.name = name;
    func->info.doc = NULL;
    func->info.params = param_names ? (PyLiteStrObject**)param_names->ob_val : NULL;
    func->info.defaults = defaults ? defaults->ob_val : NULL;
    func->info.type_codes = types;
    func->code = cfunc;

    pylt_free(param_names);
    pylt_free(defaults);
    return func;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_obj_cfunc_new(I, name, NULL, NULL, NULL, cfunc);
}
