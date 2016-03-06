
#include "module.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteObject *main_func) {
    PyLiteModuleObject *obj = pylt_realloc(NULL, sizeof(PyLiteModuleObject));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_table_new(state);
    return obj;
}

