
#include "property.h"

PyLitePropertyObject* pylt_obj_property_new(PyLiteInterpreter *I, PyLiteObject *fget, PyLiteObject *fset) {
    PyLitePropertyObject *obj = pylt_malloc(I, sizeof(PyLitePropertyObject));
    obj->ob_type = PYLT_OBJ_TYPE_PROP;
    obj->fget.func = fget;
    obj->fset.func = fset;
    return obj;
}

void pylt_obj_property_free(PyLiteInterpreter *I, PyLitePropertyObject *self) {
    pylt_free_ex(I, self);
}
