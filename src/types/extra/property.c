
#include "property.h"
#include "../../gc.h"

PyLitePropertyObject* pylt_obj_property_new(PyLiteInterpreter *I, PyLiteObject *fget, PyLiteObject *fset) {
    PyLiteObject_init(I, obj, PyLitePropertyObject, PYLT_OBJ_TYPE_PROP);
    obj->fget.func = fget;
    obj->fset.func = fset;
    if (fget) fget->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    if (fset) fset->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    obj->ob_owner = NULL;
    return obj;
}

void pylt_obj_property_rfree(PyLiteInterpreter *I, PyLitePropertyObject *self) {
    pylt_free_ex(I, self);
}

void pylt_obj_property_free(PyLiteInterpreter *I, PyLitePropertyObject *self) {
    pylt_gc_remove(I, castobj(self));
    pylt_obj_property_rfree(I, self);
}
