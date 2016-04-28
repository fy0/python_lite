
#include "custom.h"

PyLiteObject* pylt_obj_cutstom_new(PyLiteState *state, uint32_t ob_type) {
    PyLiteCustomObject *obj = pylt_realloc(NULL, sizeof(PyLiteCustomObject));
    obj->ob_type = ob_type;
    obj->ob_attrs = pylt_obj_dict_new(state);
    return castobj(obj);
}
