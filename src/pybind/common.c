
#include "common.h"
#include "../api.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_obj_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = dcast(type, args[0]);
    return pylt_obj_cutstom_new(state, type->ob_reftype, NULL);
}

PyLiteObject* pylt_method_type_mro(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject* type;
    PyLiteListObject *lst = pylt_obj_list_new(state);
    pl_uint32_t ob_type = dcast(type, args[0])->ob_reftype;

    while (ob_type) {
        type = pylt_api_gettype(state, ob_type);
        pylt_obj_list_append(state, lst, castobj(type));
        ob_type = type->ob_base;
    }

    return castobj(lst);
}

PyLiteObject* pylt_prop_type_base_get(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pylt_api_gettype(state, dcast(type, args[0])->ob_reftype);
    return castobj(pylt_api_gettype(state, type->ob_base));
}

PyLiteObject* pylt_cls_method_type_new(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_api_gettype(state, args[1]->ob_type));
}


PyLiteObject* pylt_cls_method_int_new(PyLiteState *state, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为数字
    // 奇怪。。。我当时想要干什么来着？？？好奇怪的写法
    PyLiteIntObject *num = pylt_obj_int_new(state, 0);
    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(num));
}

PyLiteObject* pylt_method_int_is_integer(PyLiteState *state, int argc, PyLiteObject **args) {
    double i;
    PyLiteFloatObject *self = dcast(float, args[0]);
    return castobj((modf(self->ob_val, &i) == 0) ? &PyLiteTrue : &PyLiteFalse);
}


PyLiteObject* pylt_cls_method_float_new(PyLiteState *state, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为数字
    PyLiteFloatObject *num = pylt_obj_float_new(state, 0);
    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(num));
}


PyLiteObject* pylt_cls_method_bool_new(PyLiteState *state, int argc, PyLiteObject **args) {
    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(pylt_obj_istrue(state, args[1]) ? &PyLiteTrue : &PyLiteFalse));
}


PyLiteObject* pylt_cls_method_str_new(PyLiteState *state, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为字符串
    PyLiteStrObject *str = pylt_obj_str_new_empty(state);
    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(str));
}

PyLiteObject* pylt_method_str_index(PyLiteState *state, int argc, PyLiteObject **args) {
    pl_int_t ret = pylt_obj_str_index_full(state, dcast(str, args[0]), dcast(str, args[1]), dcast(int, args[2])->ob_val, dcast(int, args[3])->ob_val);
    // TODO: -2
    return castobj(pylt_obj_int_new(state, ret));
}

PyLiteObject* pylt_method_str_join(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_str_join(state, dcast(str, args[0]), args[1]));
}


PyLiteObject* pylt_cls_method_bytes_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteBytesObject *str = pylt_obj_bytes_new_empty(state);
    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(str));
}


PyLiteObject* pylt_cls_method_set_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteSetObject *set = pylt_obj_set_new(state);

    if (pylt_obj_iterable(state, args[1])) {
        PyLiteIterObject *iter = pylt_obj_iter_new(state, args[1]);
        for (obj = pylt_obj_iter_next(state, iter); obj; obj = pylt_obj_iter_next(state, iter)) {
            pylt_obj_set_add(state, set, obj);
        }
    } else {
        // error
    }

    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(set));
}

PyLiteObject* pylt_method_set_add(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_add(state, dcast(set, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_set_clear(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_clear(state, dcast(set, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_set_copy(PyLiteState *state, int argc, PyLiteObject **args) {
    // 注：此处无问题，set的子类copy之后仍然返回set，cpy即如此。
    return castobj(pylt_obj_set_copy(state, dcast(set, args[0])));
}

PyLiteObject* pylt_method_set_pop(PyLiteState *state, int argc, PyLiteObject **args) {
    return pylt_obj_set_pop(state, dcast(set, args[0]));
}

PyLiteObject* pylt_method_set_remove(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_remove(state, dcast(set, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_cls_method_list_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteListObject *lst = pylt_obj_list_new(state);

    if (pylt_obj_iterable(state, args[1])) {
        PyLiteIterObject *iter = pylt_obj_iter_new(state, args[1]);
        for (obj = pylt_obj_iter_next(state, iter); obj; obj = pylt_obj_iter_next(state, iter)) {
            pylt_obj_list_append(state, lst, obj);
        }
    } else {
        // error
    }

    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(lst));
}

PyLiteObject* pylt_method_list_append(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_append(state, dcast(list, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_clear(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_clear(state, dcast(list, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_copy(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_list_copy(state, dcast(list, args[0])));
}

PyLiteObject* pylt_method_list_count(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, pylt_obj_list_count(state, dcast(list, args[0]))));
}

PyLiteObject* pylt_method_list_extend(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_extend(state, dcast(list, args[0]), dcast(list, args[1]));
    return NULL;
}

PyLiteObject* pylt_method_list_index(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, pylt_obj_list_index(state, dcast(list, args[0]), args[1])));
}

PyLiteObject* pylt_method_list_insert(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_insert(state, dcast(list, args[0]), dcast(int, args[1])->ob_val, args[2]);
    return NULL;
}

PyLiteObject* pylt_method_list_pop(PyLiteState *state, int argc, PyLiteObject **args) {
    return pylt_obj_list_pop(state, dcast(list, args[0]));
}

PyLiteObject* pylt_method_list_remove(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_remove(state, dcast(list, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_reverse(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_reverse(state, dcast(list, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_sort(PyLiteState *state, int argc, PyLiteObject **args) {
    // TODO
    return NULL;
}

PyLiteObject* pylt_cls_method_tuple_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteIterObject *iter;
    PyLiteTupleObject *tuple;
    PyLiteListObject *lst = pylt_obj_list_new(state);

    if (pylt_obj_iterable(state, args[1])) {
        iter = pylt_obj_iter_new(state, args[1]);
        for (obj = pylt_obj_iter_next(state, iter); obj; obj = pylt_obj_iter_next(state, iter)) {
            pylt_obj_list_append(state, lst, obj);
        }
    } else {
        // error
    }

    tuple = pylt_obj_tuple_new_with_data(state, pylt_obj_list_count(state, lst), (void*)lst->ob_val);
    pylt_free(lst->ob_val);

    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(tuple));
}

PyLiteObject* pylt_cls_method_dict_new(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteDictObject *kwargs = dcast(dict, args[1]);
    PyLiteDictObject *newdict = pylt_obj_dict_new(state);

    for (pl_int32_t it = pylt_obj_dict_begin(state, kwargs); it != pylt_obj_dict_end(state, kwargs); pylt_obj_dict_next(state, dcast(dict, args[1]), &it)) {
        pylt_obj_dict_setitem(state, newdict, pylt_obj_dict_itemkey(state, kwargs, it), pylt_obj_dict_itemvalue(state, kwargs, it));
    }

    return pylt_obj_typecast(state, dcast(type, args[0]), castobj(newdict));
}

