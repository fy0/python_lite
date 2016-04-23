
#include "common.h"
#include "../types/all.h"

PyLiteObject* pylt_method_obj_mro(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_method_int_is_integer(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteFloatObject *self = castfloat(args[0]);
    return castobj((modf(self->ob_val, NULL) == 0) ? &PyLiteTrue : &PyLiteFalse);
}

PyLiteObject* pylt_method_list_append(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_append(state, castlist(args[0]), args[1]);
    return NULL;
}
