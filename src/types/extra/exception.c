
#include "exception.h"
#include "../../gc.h"

PyLiteBaseExceptionObject* pylt_obj_exception_new(PyLiteInterpreter *I, PyLiteTupleObject *args) {
    PyLiteObject_init(I, obj, PyLiteBaseExceptionObject, PYLT_OBJ_TYPE_BASE_EXCEPTION);
    obj->args = args ? args : pylt_obj_tuple_new(I, 0);
    obj->args->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    return obj;
}

void pylt_obj_exception_rfree(PyLiteInterpreter *I, PyLiteBaseExceptionObject* self) {
    pylt_free_ex(I, self);
}

void pylt_obj_exception_free(PyLiteInterpreter *I, PyLiteBaseExceptionObject* self) {
    pylt_gc_remove(I, castobj(self));
    pylt_obj_exception_rfree(I, self);
}
