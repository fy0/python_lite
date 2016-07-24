
#include "common.h"
#include "../api.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_obj_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = dcast(type, args[0]);
    return pylt_obj_cutstom_new(I, type->ob_reftype, NULL);
}

PyLiteObject* pylt_method_type_mro(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTypeObject* type;
    PyLiteListObject *lst = pylt_obj_list_new(I);
    pl_uint32_t ob_type = dcast(type, args[0])->ob_reftype;

    while (ob_type) {
        type = pylt_api_gettype(I, ob_type);
        pylt_obj_list_append(I, lst, castobj(type));
        ob_type = type->ob_base;
    }

    return castobj(lst);
}

PyLiteObject* pylt_prop_type_base_get(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pylt_api_gettype(I, dcast(type, args[0])->ob_reftype);
    return castobj(pylt_api_gettype(I, type->ob_base));
}

PyLiteObject* pylt_cls_method_type_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_api_gettype(I, args[1]->ob_type));
}


PyLiteObject* pylt_cls_method_int_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为数字
    // 奇怪。。。我当时想要干什么来着？？？好奇怪的写法
    PyLiteIntObject *num = pylt_obj_int_new(I, 0);
    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(num));
}

PyLiteObject* pylt_method_int_is_integer(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    double i;
    PyLiteFloatObject *self = dcast(float, args[0]);
    return castobj((modf(self->ob_val, &i) == 0) ? &PyLiteTrue : &PyLiteFalse);
}


PyLiteObject* pylt_cls_method_float_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为数字
    PyLiteFloatObject *num = pylt_obj_float_new(I, 0);
    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(num));
}


PyLiteObject* pylt_cls_method_bool_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(pylt_obj_istrue(I, args[1]) ? &PyLiteTrue : &PyLiteFalse));
}


PyLiteObject* pylt_cls_method_str_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // TODO: 从其他形式转换为字符串
    PyLiteStrObject *str = pylt_obj_str_new_empty(I);
    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(str));
}

PyLiteObject* pylt_method_str_index(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pl_int_t ret = pylt_obj_str_index_full(I, dcast(str, args[0]), dcast(str, args[1]), dcast(int, args[2])->ob_val, dcast(int, args[3])->ob_val);
    // TODO: -2
    return castobj(pylt_obj_int_new(I, ret));
}

PyLiteObject* pylt_method_str_join(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_str_join(I, dcast(str, args[0]), args[1]));
}


PyLiteObject* pylt_cls_method_bytes_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteBytesObject *str = pylt_obj_bytes_new_empty(I);
    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(str));
}


PyLiteObject* pylt_cls_method_set_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteSetObject *set = pylt_obj_set_new(I);

    if (pylt_obj_iterable(I, args[1])) {
        PyLiteIterObject *iter = pylt_obj_iter_new(I, args[1]);
        for (obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            pylt_obj_set_add(I, set, obj);
        }
    } else {
        // error
    }

    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(set));
}

PyLiteObject* pylt_method_set_add(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_set_add(I, dcast(set, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_set_clear(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_set_clear(I, dcast(set, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_set_copy(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // 注：此处无问题，set的子类copy之后仍然返回set，cpy即如此。
    return castobj(pylt_obj_set_copy(I, dcast(set, args[0])));
}

PyLiteObject* pylt_method_set_pop(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return pylt_obj_set_pop(I, dcast(set, args[0]));
}

PyLiteObject* pylt_method_set_remove(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_set_remove(I, dcast(set, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_cls_method_list_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteListObject *lst = pylt_obj_list_new(I);

    if (pylt_obj_iterable(I, args[1])) {
        PyLiteIterObject *iter = pylt_obj_iter_new(I, args[1]);
        for (obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            pylt_obj_list_append(I, lst, obj);
        }
    } else {
        // error
    }

    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(lst));
}

PyLiteObject* pylt_method_list_append(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_append(I, dcast(list, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_clear(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_clear(I, dcast(list, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_copy(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_list_copy(I, dcast(list, args[0])));
}

PyLiteObject* pylt_method_list_count(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, pylt_obj_list_count(I, dcast(list, args[0]))));
}

PyLiteObject* pylt_method_list_extend(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_extend(I, dcast(list, args[0]), dcast(list, args[1]));
    return NULL;
}

PyLiteObject* pylt_method_list_index(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, pylt_obj_list_index(I, dcast(list, args[0]), args[1])));
}

PyLiteObject* pylt_method_list_insert(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_insert(I, dcast(list, args[0]), dcast(int, args[1])->ob_val, args[2]);
    return NULL;
}

PyLiteObject* pylt_method_list_pop(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return pylt_obj_list_pop(I, dcast(list, args[0]));
}

PyLiteObject* pylt_method_list_remove(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_remove(I, dcast(list, args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_reverse(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    pylt_obj_list_reverse(I, dcast(list, args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_sort(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // TODO
    return NULL;
}

PyLiteObject* pylt_cls_method_tuple_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteObject *obj;
    PyLiteIterObject *iter;
    PyLiteTupleObject *tuple;
    PyLiteListObject *lst = pylt_obj_list_new(I);

    if (pylt_obj_iterable(I, args[1])) {
        iter = pylt_obj_iter_new(I, args[1]);
        for (obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            pylt_obj_list_append(I, lst, obj);
        }
    } else {
        // error
    }

    tuple = pylt_obj_tuple_new_with_data(I, pylt_obj_list_count(I, lst), (void*)lst->ob_val);
    pylt_free(lst->ob_val);

    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(tuple));
}

PyLiteObject* pylt_cls_method_dict_new(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteDictObject *kwargs = dcast(dict, args[1]);
    PyLiteDictObject *newdict = pylt_obj_dict_new(I);

    for (pl_int32_t it = pylt_obj_dict_begin(I, kwargs); it != pylt_obj_dict_end(I, kwargs); pylt_obj_dict_next(I, dcast(dict, args[1]), &it)) {
        pylt_obj_dict_setitem(I, newdict, pylt_obj_dict_itemkey(I, kwargs, it), pylt_obj_dict_itemvalue(I, kwargs, it));
    }

    return pylt_obj_typecast(I, dcast(type, args[0]), castobj(newdict));
}

