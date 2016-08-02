
#ifndef PYLITE_BIND_EXTRA_H
#define PYLITE_BIND_EXTRA_H

#include "../types/object.h"

// function/cfunction
PyLiteObject* pylt_cls_method_function_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_function_defaults_get(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_function_parameters_get(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_range_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_base_exception_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_base_exception_args_get(PyLiteInterpreter *I, int argc, PyLiteObject **args);

#endif
