
#ifndef PYLITE_BIND_EXTRA_H
#define PYLITE_BIND_EXTRA_H

#include "../types/object.h"

PyLiteObject* pylt_cls_method_range_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_base_exception_new(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_base_exception_args_get(PyLiteState *state, int argc, PyLiteObject **args);

#endif
