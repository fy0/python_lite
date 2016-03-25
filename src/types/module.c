
#include "module.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeSnippet *code) {
    PyLiteModuleObject *obj = pylt_realloc(NULL, sizeof(PyLiteModuleObject));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_table_new(state);
    kv_shallowcopy(obj->const_val, code->const_val);
    kv_shallowcopy(obj->opcodes, code->opcodes);
    return obj;
}
