
#ifndef PYLITE_BIND_COMMON_H
#define PYLITE_BIND_COMMON_H

#include "../types/object.h"

PyLiteObject* pylt_cls_method_obj_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_method_type_mro(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_type_base_get(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_cls_method_type_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_int_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_int_is_integer(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_float_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_bool_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_str_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_str_index(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_str_join(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_bytes_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_set_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_add(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_clear(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_copy(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_pop(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_remove(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_list_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_append(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_clear(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_copy(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_count(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_extend(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_index(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_insert(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_pop(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_remove(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_reverse(PyLiteInterpreter *I, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_sort(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_tuple_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_dict_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

// A new function returns None
PyLiteObject* pylt_cls_method_useless_new(PyLiteInterpreter *I, int argc, PyLiteObject **args);

#endif
