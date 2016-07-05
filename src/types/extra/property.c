
#include "property.h"

PyLitePropertyObject* pylt_obj_property_new(PyLiteState *state, PyLiteObject *fget, PyLiteObject *fset) {
    PyLitePropertyObject *obj = pylt_realloc(NULL, sizeof(PyLitePropertyObject));
    obj->ob_type = PYLT_OBJ_TYPE_PROP;
    obj->fget.func = fget;
    obj->fset.func = fset;
    return obj;
}

void pylt_obj_property_free(PyLiteState *state, PyLitePropertyObject *self) {
    pylt_free(self);
}
