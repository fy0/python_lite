
#include "type.h"
#include "dict.h"

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, pl_uint32_t base) {
    PyLiteTypeObject *obj = pylt_realloc(NULL, sizeof(PyLiteTypeObject));
    obj->ob_type = PYLT_OBJ_TYPE_TYPE;
    obj->ob_attrs = pylt_obj_dict_new(state);
    obj->ob_base = base;
    return obj;
}
