
#include "function.h"
#include "../../vm.h"
#include "../../bind.h"
#include "../../intp.h"

PyLiteDictObject* make_closure(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteContext *ctx = I->vm.ctx;
    PyLiteDictObject *closure = pylt_obj_dict_new(I);

    if (!code || !code->closure) return closure;
    pl_foreach_set(I, it, code->closure) {
        PyLiteObject *obj = NULL;
        PyLiteObject *name = pylt_obj_set_itemvalue(I, code->closure, it);
        for (int i = kv_size(ctx->frames) - 1; i >= 0; --i) {
            obj = pylt_obj_dict_getitem(I, kv_A(ctx->frames, i).locals, name);
            if (obj) {
                pylt_obj_dict_setitem(I, closure, name, obj);
                break;
            }
        }
    }

    return closure;
}

PyLiteFunctionObject* pylt_obj_func_new(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteObject_init(I, obj, PyLiteFunctionObject, PYLT_OBJ_TYPE_FUNCTION);
    memset(&obj->info, 0, sizeof(PyLiteFunctionInfo));
    obj->ob_owner = NULL;

    if (code) {
        memcpy(&obj->code, code, sizeof(PyLiteCodeObject));
    } else {
        obj->code.const_val = pylt_obj_list_new(I);
        kv_init(I, obj->code.opcodes);
    }
    obj->info.closure = make_closure(I, code);
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
    PyLiteObject_init(I, func, PyLiteCFunctionObject, PYLT_OBJ_TYPE_CFUNCTION);
    func->info.length = param_names ? param_names->ob_size : 0;
    func->info.minimal = func->info.length;
    if (defaults) {
        pl_int_t skip = 0;
        for (pl_int_t i = 0; i < defaults->ob_size; ++i) {
            if (defaults->ob_val[i] == castobj(&PyLiteParamUndefined)) skip++;
            else break;
        } 
        func->info.minimal -= (defaults->ob_size - skip);
    }

    func->ob_owner = NULL;
    func->info.name = name;
    func->info.doc = NULL;
    func->info.params = param_names ? param_names : NULL;
    func->info.defaults = defaults ? defaults : NULL;
    func->info.type_codes = types;
    func->info.closure = NULL;
    func->code = cfunc;

    return func;
}

PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc) {
    return pylt_obj_cfunc_new(I, name, NULL, NULL, NULL, cfunc);
}
