
#include "function.h"
#include "../../bind.h"

PyLiteFunctionObject* pylt_obj_func_new(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteFunctionObject *obj = pylt_malloc(I, sizeof(PyLiteFunctionObject));
    memset(obj, 0, sizeof(PyLiteFunctionObject));
    obj->ob_type = PYLT_OBJ_TYPE_FUNCTION;
    obj->ob_flags = 0;
    if (code) {
        memcpy(&obj->code, code, sizeof(PyLiteCodeObject));
    } else {
        obj->code.const_val = pylt_obj_list_new(I);
        kv_init(I, obj->code.opcodes);
    }
    return obj;
}

PyLiteFunctionObject* pylt_obj_func_new_ex(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteListObject *params, PyLiteCodeObject *code, PyLiteDictObject *defaults, PyLiteStrObject *args, PyLiteStrObject *kwargs) {
    PyLiteFunctionObject *func = pylt_obj_func_new(I, code);

    // set name and params
    func->info.name = name;
    func->info.length = params->ob_size;
    func->info.params = pylt_obj_tuple_new(I, params->ob_size);
    memcpy(func->info.params->ob_val, params->ob_val, params->ob_size * sizeof(PyLiteObject*));

    // set default values
    if (defaults || args || kwargs) {
        pl_int_t minimal = -1;
		PyLiteTupleObject *defvals = pylt_obj_tuple_new(I, params->ob_size);

        for (int i = 0; i < params->ob_size; ++i) {
            PyLiteStrObject *name = caststr(params->ob_val[i]);
            if (name == args) {
                if (minimal == -1) minimal = i;
                defvals->ob_val[i] = castobj(&PyLiteParamArgs);
            } else if (name == kwargs) {
                if (minimal == -1) minimal = i;
				defvals->ob_val[i] = castobj(&PyLiteParamKwargs);
			} else if (defaults && pylt_obj_dict_has(I, defaults, castobj(name))) {
				if (minimal == -1) minimal = i;
				defvals->ob_val[i] = pylt_obj_dict_getitem(I, defaults, castobj(name));
			} else defvals->ob_val[i] = castobj(&PyLiteParamUndefined);
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
    PyLiteCFunctionObject *func = pylt_malloc(I, sizeof(PyLiteCFunctionObject));
    func->ob_type = PYLT_OBJ_TYPE_CFUNCTION;
    func->ob_flags = 0;
    func->info.length = param_names ? param_names->ob_size : 0;
    func->info.minimal = func->info.length - (defaults ? defaults->ob_size : 0);

    func->info.name = name;
    func->info.doc = NULL;
	func->info.params = param_names ? param_names : NULL;
    func->info.defaults = defaults ? defaults : NULL;
    func->info.type_codes = types;
    func->code = cfunc;

    return func;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_obj_cfunc_new(I, name, NULL, NULL, NULL, cfunc);
}
