
#ifndef PYLITE_BIND_OBJ_H
#define PYLITE_BIND_OBJ_H

#include "../types/object.h"

PyLiteObject* pylt_method_obj_mro(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_method_int_is_integer(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_method_list_append(PyLiteState *state, int argc, PyLiteObject **args);


#endif
