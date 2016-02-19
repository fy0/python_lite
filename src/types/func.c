
#include "func.h"

PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state) {
    PyLiteFunctionObject *obj = pylt_realloc(NULL, sizeof(PyLiteFunctionObject*));
    obj->ob_type = PYLT_OBJ_TYPE_FUNCTION;
    obj->locals = pylt_obj_table_new(state);
    kv_init(obj->const_val);
    kv_init(obj->opcodes);
    return obj;
}
