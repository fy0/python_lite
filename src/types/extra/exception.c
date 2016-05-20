
#include "exception.h"

PyLiteExceptionObject* pylt_obj_exception_new(PyLiteState *state, PyLiteTupleObject *args) {
    PyLiteExceptionObject *obj = pylt_realloc(NULL, sizeof(PyLiteExceptionObject));
    obj->ob_type = PYLT_OBJ_TYPE_EXCEPTION;
    obj->args = args ? args : pylt_obj_tuple_new(state, 0);
    return obj;
}
