
#include "exception.h"

PyLiteBaseExceptionObject* pylt_obj_exception_new(PyLiteInterpreter *I, PyLiteTupleObject *args) {
    PyLiteBaseExceptionObject *obj = pylt_malloc(I, sizeof(PyLiteBaseExceptionObject));
    obj->ob_type = PYLT_OBJ_TYPE_BASE_EXCEPTION;
    obj->ob_flags = 0;
    obj->args = args ? args : pylt_obj_tuple_new(I, 0);
    return obj;
}

void pylt_obj_exception_free(PyLiteInterpreter *I, PyLiteBaseExceptionObject* self) {
    pylt_free_ex(I, self);
}
