
#include "function.h"

PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state) {
    PyLiteFunctionObject *obj = pylt_realloc(NULL, sizeof(PyLiteFunctionObject));
    memset(obj, 0, sizeof(PyLiteFunctionObject));
    obj->ob_type = PYLT_OBJ_TYPE_FUNCTION;
    kv_init(obj->code.const_val);
    kv_init(obj->code.opcodes);
    return obj;
}
