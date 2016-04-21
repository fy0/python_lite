
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
    obj->info.with_args = -1;
    return obj;
}

PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteState *state, PyLiteObject *func) {
    if (func->ob_type == PYLT_OBJ_TYPE_FUNCTION)
        return &castfunc(func)->info;
    if (func->ob_type == PYLT_OBJ_TYPE_CFUNCTION)
        return &castcfunc(func)->info;
    return NULL;
}
