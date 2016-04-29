
#include "module.h"
#include "dict.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeObject *code) {
    PyLiteModuleObject *obj = castmod(pylt_realloc(NULL, sizeof(PyLiteModuleObject)));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_dict_new(state);
    obj->code = code;
    return obj;
}
