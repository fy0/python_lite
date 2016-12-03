
#include "with_exceptions.h"
#include "../api.h"

typedef PyLiteObject* (*GetItemFunc)(PyLiteInterpreter *I, PyLiteObject *self, int index);

PyLiteObject* pylt_obj_X_Egetitem(PyLiteInterpreter *I, GetItemFunc func, PyLiteObject *self, PyLiteObject *index) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        PyLiteObject *ret = (*func)(I, self, castint(index)->ob_val);
        if (!ret) pl_error(I, pl_static.str.IndexError, "%s index out of range", pl_type(I, castobj(self))->name);
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return NULL;
}


PyLiteObject* pylt_obj_tuple_Egetitem(PyLiteInterpreter *I, PyLiteTupleObject *self, PyLiteObject *index) {
    return pylt_obj_X_Egetitem(I, (GetItemFunc)&pylt_obj_tuple_getitem, castobj(self), index);
}

PyLiteObject* pylt_obj_list_Egetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index) {
    return pylt_obj_X_Egetitem(I, (GetItemFunc)&pylt_obj_list_getitem, castobj(self), index);
}

PyLiteObject* pylt_obj_dict_Egetitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    PyLiteObject *ret = pylt_obj_dict_getitem(I, self, key);
    if (!ret) pl_error(I, pl_static.str.KeyError, "%s", key);
    return ret;
}

PyLiteBytesObject* pylt_obj_bytes_Egetitem(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *index) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        PyLiteBytesObject *ret = pylt_obj_bytes_getitem(I, self, castint(index)->ob_val);
        if (!ret) pl_error(I, pl_static.str.IndexError, "list index out of range");
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return NULL;
}

PyLiteStrObject* pylt_obj_str_Egetitem(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *index) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        PyLiteStrObject *ret = pylt_obj_str_getitem(I, self, castint(index)->ob_val);
        if (!ret) pl_error(I, pl_static.str.IndexError, "list index out of range");
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return NULL;
}

pl_bool_t pylt_obj_list_Esetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index, PyLiteObject* obj) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        pl_bool_t ret = pylt_obj_list_setitem(I, self, castint(index)->ob_val, obj);
        if (!ret) pl_error(I, pl_static.str.IndexError, "list assignment index out of range");
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return false;
}

pl_bool_t pylt_obj_dict_Eremove(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    if (!pylt_obj_dict_remove(I, self, key)) {
        pl_error(I, pl_static.str.KeyError, "%s", key);
        return false;
    }
    return true;
}

pl_bool_t pylt_obj_Esetitem(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    switch (self->ob_type) {
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_Esetitem(I, castlist(self), key, value);
        case PYLT_OBJ_TYPE_DICT:
            pylt_obj_dict_setitem(I, castdict(self), key, value);
            return true;
    }
    return false;
}

PyLiteObject* pylt_obj_Egetitem(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            return castobj(pylt_obj_bytes_Egetitem(I, castbytes(obj), key));
        case PYLT_OBJ_TYPE_STR:
            return castobj(pylt_obj_str_Egetitem(I, caststr(obj), key));
        case PYLT_OBJ_TYPE_LIST:
            return pylt_obj_list_Egetitem(I, castlist(obj), key);
        case PYLT_OBJ_TYPE_TUPLE:
            return pylt_obj_tuple_Egetitem(I, casttuple(obj), key);
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_Egetitem(I, castdict(obj), key);
        default:
            if (pl_iscustom(obj)) {
                PyLiteObject *method_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__getitem__), NULL);
                if (method_func) {
                    return pl_call_method(I, obj, method_func, 1, key);
                } else {
                    if (castcustom(obj)->base_obj) {
                        return pylt_obj_Egetitem(I, castcustom(obj)->base_obj, key);
                    }
                }
            }
    }
    return NULL;
}

pl_bool_t pylt_obj_Edelitem(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_LIST:
            if (key->ob_type == PYLT_OBJ_TYPE_INT) {
                pl_bool_t ret = pylt_obj_list_delitem(I, obj, castint(key)->ob_val);
                if (!ret) pl_error(I, pl_static.str.IndexError, "%s index out of range", pl_type(I, castobj(key))->name);
                return ret;
            }
            pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(obj))->name, pl_type(I, key)->name);
            return false;
        case PYLT_OBJ_TYPE_DICT:
            return pylt_obj_dict_Eremove(I, castdict(obj), key);
        default:
            if (pl_iscustom(obj)) {
                PyLiteObject *method_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__delitem__), NULL);
                if (method_func) {
                    return pylt_obj_istrue(I, pl_call_method(I, obj, method_func, 1, key));
                }
            }
    }
    return false;
}

PyLiteIterObject* pylt_obj_iter_Enew(PyLiteInterpreter *I, PyLiteObject *obj) {
    PyLiteIterObject *ret = pylt_obj_iter_new(I, obj);
    if (!ret) {
        pl_error(I, pl_static.str.TypeError, "%r object is not iterable", pl_type(I, obj)->name);
        return NULL;
    }
    if (!pl_api_issubclass(I, pl_type(I, castobj(ret)), pl_type_by_code(I, PYLT_OBJ_TYPE_ITER))) {
        pl_error(I, pl_static.str.TypeError, "iter() returned non-iterator of type %r", pl_type(I, castobj(ret))->name);
        return NULL;
    }
    return ret;
}

PyLiteObject* pylt_obj_iter_Enext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    if (!pl_api_issubclass(I, pl_type(I, castobj(iter)), pl_type_by_code(I, PYLT_OBJ_TYPE_ITER))) {
        pl_error(I, pl_static.str.TypeError, "%r object is not iterable", pl_type(I, castobj(iter))->name);
        return NULL;
    }
    PyLiteObject *obj = pylt_obj_iter_next(I, iter);
    if (!obj) {
        pl_error(I, pl_static.str.StopIteration, NULL);
        return NULL;
    }
    return obj;
}

PyLiteObject* pylt_obj_Egetattr_ex(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, PyLiteObject* _default, pl_bool_t *p_at_type) {
    PyLiteObject *ret = pylt_obj_getattr_ex(I, obj, key, _default, p_at_type);
    if (!ret) {
        // if default value is &PyLiteUseless, not found and no exception
        if (_default) return _default;
        pl_error(I, pl_static.str.AttributeError, "type object %r has no attribute %r",
            pylt_api_type_name(I, obj->ob_type), key);
    }
    return ret;
}

PyLiteObject* pylt_obj_Egetattr(PyLiteInterpreter *I, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type) {
    PyLiteObject *ret = pylt_obj_Egetattr_ex(I, obj, key, castobj(&PyLiteUseless), p_at_type);
    return (ret != castobj(&PyLiteUseless)) ? ret : NULL;
}

pl_bool_t pylt_obj_Esetattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value) {
    if (pl_iscustom(self)) {
        PyLiteObject *method_func = pylt_obj_getattr(I, self, castobj(pl_static.str.__setattr__), NULL);
        if (method_func) {
            PyLiteObject *ret = pl_call_method(I, self, method_func, 2, key, value);
            return pylt_obj_istrue(I, ret);
        }
    }
    return pylt_obj_setattr(I, self, key, value);
}

pl_bool_t pylt_obj_Edelattr(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject* key) {
    if (pl_iscustom(self)) {
        PyLiteObject *method_func = pylt_obj_getattr(I, self, castobj(pl_static.str.__delattr__), NULL);
        if (method_func) {
            PyLiteObject *ret = pl_call_method(I, self, method_func, 1, key);
            return pylt_obj_istrue(I, ret);
        }
    }
    return pylt_obj_delattr(I, self, key);
}

pl_int_t pylt_obj_Ecmp(PyLiteInterpreter *I, PyLiteObject *a, PyLiteObject *b) {
    if (a == b) return 0;
    switch (a->ob_type) {
        case PYLT_OBJ_TYPE_INT: return pylt_obj_int_cmp(I, castint(a), b);
        case PYLT_OBJ_TYPE_FLOAT: return pylt_obj_float_cmp(I, castfloat(a), b);
        case PYLT_OBJ_TYPE_BOOL: return pylt_obj_bool_cmp(I, castbool(a), b);
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_cmp(I, caststr(a), b);
        case PYLT_OBJ_TYPE_BYTES: return pylt_obj_bytes_cmp(I, castbytes(a), b);
        case PYLT_OBJ_TYPE_SET: return pylt_obj_set_cmp(I, castset(a), b);
        case PYLT_OBJ_TYPE_DICT: return pylt_obj_dict_cmp(I, castdict(a), b);
        default:
            if (a->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteObject *hash_func = pylt_obj_getattr(I, a, castobj(pl_static.str.__cmp__), NULL);
                if (hash_func) {
                    PyLiteObject *ret = pl_call_method(I, a, hash_func, 1, b);
                    if (ret->ob_type == PYLT_OBJ_TYPE_INT) {
                        return castint(ret)->ob_val;
                    }
                }
            }
    }
    return 2;
}

PyLiteObject* pylt_obj_Eslice(PyLiteInterpreter *I, PyLiteObject *obj, pl_int_t start, pl_int_t end, pl_int_t step) {
    if (step == 0) {
        pl_error(I, pl_static.str.ValueError, "slice step cannot be zero");
        return NULL;
    }

    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_LIST:
            return castobj(pylt_obj_list_slice(I, castlist(obj), start, end, step));
        case PYLT_OBJ_TYPE_TUPLE:
            return castobj(pylt_obj_tuple_slice(I, casttuple(obj), start, end, step));
    }

    pl_error(I, pl_static.str.TypeError, "%r object has no attribute '__getitem__'", pl_type(I, obj)->name);
    return NULL;
}

void pylt_obj_Eslice_set(PyLiteInterpreter *I, PyLiteObject *obj, pl_int_t start, pl_int_t end, pl_int_t step, PyLiteObject *val) {
    if (step == 0) {
        pl_error(I, pl_static.str.ValueError, "slice step cannot be zero");
        return;
    }

    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_LIST: {
            pl_int_t ret = pylt_obj_list_slice_set(I, castlist(obj), start, end, step, val);
            // TODO
            if (ret == -2) pl_error(I, pl_static.str.ValueError, "attempt to assign sequence of size X to extended slice of size Y");
            return;
        }
    }

    pl_error(I, pl_static.str.TypeError, "%r object does not support item assignment", pl_type(I, obj)->name);
    return;
}


PyLiteStrObject* pylt_obj_Eto_str(PyLiteInterpreter *I, PyLiteObject *obj) {
    if (pl_iscustom(obj)) {
        PyLiteObject *method_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__str__), NULL);
        if (method_func) {
            PyLiteObject *ret = pl_call_method(I, obj, method_func, 0);
            if (!pylt_api_isinstance(I, ret, PYLT_OBJ_TYPE_STR)) {
                pl_error(I, pl_static.str.TypeError, "__str__ returned non - string(type %s)", pl_type(I, ret)->name);
                return NULL;
            }
            return caststr(ret);
        }
    }
    return pylt_obj_to_str(I, obj);
}

PyLiteStrObject* pylt_obj_Eto_repr(PyLiteInterpreter *I, PyLiteObject *obj) {
    // TODO
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_STR: return pylt_obj_str_to_repr(I, caststr(obj));
        default: return pylt_obj_to_str(I, obj);
    }
    return NULL;
}
