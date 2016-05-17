
#include "exception.h"

PyLiteExceptionObject* pylt_obj_exception_new(PyLiteState *state) {
    PyLiteExceptionObject *obj = pylt_realloc(NULL, sizeof(PyLiteExceptionObject));
    obj->ob_type = PYLT_OBJ_TYPE_EXCEPTION;
    obj->args = NULL;
    return obj;
}
