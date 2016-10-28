
#include "extra.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_function_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // won't supported
    return NULL;
}

PyLiteObject* pylt_prop_function_args_types_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFunctionInfo *info = pylt_obj_func_get_info(I, args[0]);
    PyLiteTupleObject *data = pylt_obj_tuple_new(I, info->length);
    if (!info->type_codes) {
        for (pl_int_t i = 0; i < data->ob_size; ++i) {
            data->ob_val[i] = castobj(pylt_api_gettype_by_code(I, PYLT_OBJ_TYPE_OBJ));
        }
    } else {
        for (pl_int_t i = 0; i < data->ob_size; ++i) {
            pl_int_t typecode = info->type_codes[i];
            data->ob_val[i] = castobj(pylt_api_gettype_by_code(I, typecode ? typecode : PYLT_OBJ_TYPE_OBJ));
        }
    }
    return castobj(data);
}

PyLiteObject* pylt_prop_function_defaults_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFunctionInfo *info = pylt_obj_func_get_info(I, args[0]);
    return castobj(info->defaults);
}

PyLiteObject* pylt_prop_function_parameters_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteFunctionInfo *info = pylt_obj_func_get_info(I, args[0]);
    return castobj(info->params);
}

PyLiteObject* pylt_cls_method_iter_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (!pl_bind_cls_check(I, pl_type_by_code(I, PYLT_OBJ_TYPE_ITER), _S(__new__), casttype(args[0])))
        return NULL;
    return pylt_obj_typecast(I, casttype(args[0]), castobj(pylt_obj_iter_new(I, args[1])));
}

PyLiteObject* pylt_cls_method_range_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_int_t start, end;
    if (!pl_bind_cls_check(I, pl_type_by_code(I, PYLT_OBJ_TYPE_RANGE), _S(__new__), casttype(args[0])))
        return NULL;

    if (castnone(args[2]) == &PyLiteNone) {
        start = 0;
        end = dcast(int, args[1])->ob_val;
    } else {
        start = dcast(int, args[1])->ob_val;
        end = dcast(int, args[2])->ob_val;
    }

    return pylt_obj_typecast(
        I,
        dcast(type, args[0]),
        castobj(pylt_obj_range_new(I, start, end, dcast(int, args[3])->ob_val))
    );
}

PyLiteObject* pylt_cls_method_base_exception_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (!pl_bind_cls_check(I, pl_type_by_code(I, PYLT_OBJ_TYPE_BASE_EXCEPTION), _S(__new__), casttype(args[0])))
        return NULL;

    return pylt_obj_typecast(
        I, 
        dcast(type, args[0]),
        castobj(pylt_obj_exception_new(I, dcast(tuple, args[1])))
    );
}

PyLiteObject* pylt_prop_base_exception_args_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(dcast(except, args[1])->args);
}
