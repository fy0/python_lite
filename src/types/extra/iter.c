
#include "iter.h"
#include "../all.h"
#include "../../vm.h"
#include "../../misc.h"

PyLiteIterObject* pylt_obj_iter_new(PyLiteInterpreter *I, PyLiteObject *obj) {
    PyLiteIterObject *iter;
    if (obj->ob_type == PYLT_OBJ_TYPE_ITER) return castiter(obj);

    iter = pylt_realloc(NULL, sizeof(PyLiteIterObject));
    iter->ob_type = PYLT_OBJ_TYPE_ITER;
    iter->base = obj;
    iter->backup = NULL;

    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            iter->array.count = castbytes(obj)->ob_size;
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_bytes_iternext;
            return iter;
        case PYLT_OBJ_TYPE_STR:
            iter->array.count = caststr(obj)->ob_size;
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_str_iternext;
            return iter;
        case PYLT_OBJ_TYPE_TUPLE:
            iter->array.count = casttuple(obj)->ob_size;
            iter->array.index = 0;
            iter->iter_func = &pylt_obj_tuple_iternext;
            return iter;
        case PYLT_OBJ_TYPE_LIST:
            iter->array.count = castlist(obj)->ob_size;
            iter->iter_func = &pylt_obj_list_iternext;
            iter->array.index = 0;
            return iter;
        case PYLT_OBJ_TYPE_SET:
            iter->hashmap.count = pylt_obj_set_len(I, castset(obj));
            iter->hashmap.k = pylt_obj_set_begin(I, castset(obj));
            iter->iter_func = &pylt_obj_set_iternext;
            return iter;
		case PYLT_OBJ_TYPE_DICT:
			iter->hashmap.count = pylt_obj_dict_len(I, castdict(obj));
			iter->hashmap.k = pylt_obj_dict_begin(I, castdict(obj));
			iter->iter_func = &pylt_obj_dict_iternext;
			return iter;
        case PYLT_OBJ_TYPE_RANGE:
            iter->array.count = pylt_obj_range_itertimes(I, castrange(obj));
            iter->array.index = castrange(obj)->start;
            iter->iter_func = &pylt_obj_range_iternext;
            return iter;
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                PyLiteIterObject *niter;
                PyLiteObject *obj_func = pylt_obj_getattr(I, obj, castobj(pl_static.str.__iter__), NULL);
                if (obj_func) {
                    niter = castiter(pylt_vm_call_method(I, obj, obj_func, 0, NULL));
                    if (pl_isiter(niter)) {
                        pylt_free(iter);
                        return iter;
                    }
                } else {
                    iter->backup = pylt_obj_iter_new(I, castcustom(obj)->base_obj);
                    if (iter->backup) {
                        iter->iter_func = &pylt_obj_custom_iternext;
                        return iter;
                    }
                }
            }
    }

    pylt_free(iter);
    return NULL;
}

PyLiteObject* pylt_obj_iter_next(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    return (*iter->iter_func)(I, iter);
}

PyLiteObject* pylt_obj_bytes_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    char buf[1];
    int len = castbytes(iter->base)->ob_size;
    if (iter->array.index == len) return NULL;
    else {
        buf[0] = castbytes(iter->base)->ob_val[iter->array.index++];
        return castobj(pylt_obj_bytes_new(I, buf, 1, true));
    }
    return NULL;
}

PyLiteObject* pylt_obj_str_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    uint32_t buf[1];
    int len = caststr(iter->base)->ob_size;
    if (iter->array.index == len) return NULL;
    else {
        buf[0] = caststr(iter->base)->ob_val[iter->array.index++];
        return castobj(pylt_obj_str_new(I, buf, 1, true));
    }
    return NULL;
}

PyLiteObject* pylt_obj_tuple_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    int len = casttuple(iter->base)->ob_size;
    if (iter->array.index < len) {
        return casttuple(iter->base)->ob_val[iter->array.index++];
    }
    return NULL;
}

PyLiteObject* pylt_obj_list_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    int len = castlist(iter->base)->ob_size;
    if (iter->array.index < len) {
        return castlist(iter->base)->ob_val[iter->array.index++];
    }
    return NULL;
}

PyLiteObject* pylt_obj_set_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    PyLiteObject *ret;

    if (pylt_obj_set_len(I, castset(iter->base)) != iter->hashmap.count)
        return NULL;

    ret = pylt_obj_set_itemvalue(I, castset(iter->base), iter->hashmap.k);
    pylt_obj_set_next(I, castset(iter->base), &(iter->hashmap.k));
    return ret;
}

PyLiteObject* pylt_obj_dict_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
	PyLiteObject *ret;

	if (pylt_obj_dict_len(I, castdict(iter->base)) != iter->hashmap.count)
		return NULL;

	ret = pylt_obj_dict_itemkey(I, castdict(iter->base), iter->hashmap.k);
	pylt_obj_dict_next(I, castdict(iter->base), &(iter->hashmap.k));
	return ret;
}

PyLiteObject* pylt_obj_range_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    PyLiteRangeObject *range = castrange(iter->base);
    pl_int_t index = iter->array.index;
    if ((index >= range->start) && (index < range->stop)) {
        iter->array.index += range->step;
        return castobj(pylt_obj_int_new(I, index));
    }
    return NULL;
}

PyLiteObject* pylt_obj_custom_iternext(PyLiteInterpreter *I, PyLiteIterObject *iter) {
    return pylt_obj_iter_next(I, iter->backup);
}

void pylt_obj_iter_free(PyLiteInterpreter *I, PyLiteIterObject* self) {
    if (self->backup) pylt_obj_iter_free(I, self->backup);
    pylt_free(self);
}
