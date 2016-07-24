
#include "exception.h"

PyLiteBaseExceptionObject* pylt_obj_exception_new(PyLiteInterpreter *I, PyLiteTupleObject *args) {
    PyLiteBaseExceptionObject *obj = pylt_realloc(NULL, sizeof(PyLiteBaseExceptionObject));
    obj->ob_type = PYLT_OBJ_TYPE_BASE_EXCEPTION;
    obj->args = args ? args : pylt_obj_tuple_new(I, 0);
    return obj;
}

void pylt_obj_exception_free(PyLiteInterpreter *I, PyLiteBaseExceptionObject* self) {
    pylt_free(self);
}
