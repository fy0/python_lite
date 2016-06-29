
#ifndef PYLITE_BIND_COMMON_H
#define PYLITE_BIND_COMMON_H

#include "../types/object.h"

PyLiteObject* pylt_cls_method_obj_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_method_type_mro(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_prop_type_base_get(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_cls_method_type_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_int_new(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_int_is_integer(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_float_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_bool_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_str_new(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_str_index(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_str_join(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_bytes_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_set_new(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_add(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_clear(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_copy(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_pop(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_set_remove(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_list_new(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_append(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_clear(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_copy(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_count(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_extend(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_index(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_insert(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_pop(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_remove(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_reverse(PyLiteState *state, int argc, PyLiteObject **args);
PyLiteObject* pylt_method_list_sort(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_tuple_new(PyLiteState *state, int argc, PyLiteObject **args);

PyLiteObject* pylt_cls_method_dict_new(PyLiteState *state, int argc, PyLiteObject **args);

#endif
