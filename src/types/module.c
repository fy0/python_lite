
#include "module.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, VecObject const_val, VecOpcode opcodes) {
    PyLiteModuleObject *obj = pylt_realloc(NULL, sizeof(PyLiteModuleObject));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_table_new(state);
    kv_shallowcopy(obj->const_val, const_val);
    kv_shallowcopy(obj->opcodes, opcodes);
    return obj;
}
