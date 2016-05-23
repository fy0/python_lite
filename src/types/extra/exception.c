
#include "exception.h"

PyLiteBaseExceptionObject* pylt_obj_base_exception_new(PyLiteState *state, PyLiteTupleObject *args) {
    PyLiteBaseExceptionObject *obj = pylt_realloc(NULL, sizeof(PyLiteBaseExceptionObject));
    obj->ob_type = PYLT_OBJ_TYPE_BASE_EXCEPTION;
    obj->args = args ? args : pylt_obj_tuple_new(state, 0);
    return obj;
}

void pylt_obj_exception_free(PyLiteState *state, PyLiteBaseExceptionObject* self) {
    pylt_free(self);
}
