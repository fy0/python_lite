
#include "property.h"

PyLitePropertyObject* pylt_obj_property_new(PyLiteState *state, PyLiteObject *func) {
    PyLitePropertyObject *obj = pylt_realloc(NULL, sizeof(PyLitePropertyObject));
    obj->ob_type = PYLT_OBJ_TYPE_PROP;
    obj->func = func;
}
