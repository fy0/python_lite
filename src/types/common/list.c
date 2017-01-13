
#include "list.h"
#include "string.h"
#include "../extra/iter.h"
#include "../../utils/misc.h"

// 将 index 转为标准形式并约束到可用范围
#define index_fix(__index) \
    if (__index < 0) __index += self->ob_size; \
    if (__index < 0) __index = 0; \
    else if (__index >= self->ob_size) __index = self->ob_size;


#define index_chk(__index, failret) \
    if (__index < 0) __index += self->ob_size; \
    if (__index < 0 || __index >= self->ob_size) return failret;


void list_resize_maxsize_more_then(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t size) {
    if (size > self->ob_maxsize) {
        self->ob_val = pylt_realloc(I, self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*), size * sizeof(PyLiteObject*));
        self->ob_maxsize = size;
    }
}

PyLiteListObject* pylt_obj_list_new(PyLiteInterpreter *I) {
    PyLiteListObject *obj = pylt_malloc(I, sizeof(PyLiteListObject));
    obj->ob_type = PYLT_OBJ_TYPE_LIST;
    obj->ob_flags = 0;
    obj->ob_size = 0;
    obj->ob_maxsize = 4;
    obj->ob_val = pylt_malloc(I, 4 * sizeof(PyLiteObject*));
    return obj;
}

PyLiteListObject* pylt_obj_list_new_with_size(PyLiteInterpreter *I, pl_uint_t size) {
    PyLiteListObject *obj = pylt_malloc(I, sizeof(PyLiteListObject));
    obj->ob_type = PYLT_OBJ_TYPE_LIST;
    obj->ob_size = 0;
    obj->ob_maxsize = size;
    obj->ob_val = (size) ? pylt_malloc(I, size * sizeof(PyLiteObject*)) : NULL;
    return obj;
}

void pylt_obj_list_free(PyLiteInterpreter *I, PyLiteListObject *self) {
    pylt_free(I, self->ob_val, sizeof(PyLiteObject*) * self->ob_maxsize);
    pylt_free_ex(I, self);
}

pl_int_t pylt_obj_list_append(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj) {
    if (self->ob_size >= self->ob_maxsize) {
        // sometimes new with size 0
        pl_int_t old_size = self->ob_maxsize;
        if (self->ob_maxsize == 0) self->ob_maxsize = 4;
        else self->ob_maxsize *= 2;
        self->ob_val = pylt_realloc(I, self->ob_val, old_size * sizeof(PyLiteObject*), self->ob_maxsize * sizeof(PyLiteObject*));
    }
    self->ob_val[self->ob_size++] = obj;
    return self->ob_size - 1;
}

void pylt_obj_list_clear(PyLiteInterpreter *I, PyLiteListObject *self) {
    self->ob_size = 0;
}

PyLiteListObject* pylt_obj_list_copy(PyLiteInterpreter *I, PyLiteListObject *self) {
    PyLiteListObject *obj = pylt_malloc(I, sizeof(PyLiteListObject));
    memcpy(obj, self, sizeof(PyLiteListObject));
    obj->ob_val = pylt_malloc(I, obj->ob_size * sizeof(PyLiteObject*));
    memcpy(obj->ob_val, self->ob_val, obj->ob_size * sizeof(PyLiteObject*));
    return obj;
}

pl_uint_t pylt_obj_list_count(PyLiteInterpreter *I, PyLiteListObject *self) {
    return self->ob_size;
}

void pylt_obj_list_extend(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteListObject *obj) {
    if (self->ob_size + obj->ob_size >= self->ob_maxsize) {
        pl_int_t old_size = self->ob_maxsize;
        self->ob_maxsize = self->ob_size + obj->ob_size;
        self->ob_val = pylt_realloc(I, self->ob_val, old_size * sizeof(PyLiteObject*), self->ob_maxsize * sizeof(PyLiteObject*));
    }
    memcpy(self->ob_val + self->ob_size, obj->ob_val, obj->ob_size * sizeof(PyLiteObject*));
    self->ob_size = self->ob_size + obj->ob_size;
}

pl_int_t pylt_obj_list_index(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_eq(I, self->ob_val[i], obj)) return i;
    }
    return -1;
}

// 同时要求类型相同，例如 1 == 1.0 但仍有区别
pl_int_t pylt_obj_list_index_strict(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if ((self->ob_val[i]->ob_type == obj->ob_type) && (pylt_obj_eq(I, self->ob_val[i], obj))) {
            return i;
        }
    }
    return -1;
}

pl_bool_t pylt_obj_list_insert(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t index, PyLiteObject *obj) {
    index_chk(index, false);

    if (self->ob_size + 1 >= self->ob_maxsize) {
        pl_int_t old_size = self->ob_maxsize;
        self->ob_maxsize = self->ob_size + 1;
        self->ob_val = pylt_realloc(I, self->ob_val, old_size * sizeof(PyLiteObject*), self->ob_maxsize * sizeof(PyLiteObject*));
    }

    memcpy(self->ob_val + index + 1, self->ob_val + index, (self->ob_size - index + 1) * sizeof(PyLiteObject*));
    self->ob_val[index] = obj;
    return true;
}

pl_bool_t pylt_obj_list_remove(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_eq(I, self->ob_val[i], obj)) {
            pylt_obj_list_delitem(I, self, i);
            return true;
        }
    }
    return false;
}

void pylt_obj_list_reverse(PyLiteInterpreter *I, PyLiteListObject *self) {
    pl_int_t halfsize;
    PyLiteObject *tmp;
    if (self->ob_size > 1) {
        halfsize = self->ob_size / 2;
        for (pl_int_t i = 0; i < halfsize; ++i) {
            tmp = self->ob_val[i];
            self->ob_val[i] = self->ob_val[self->ob_size - i - 1];
            self->ob_val[self->ob_size - i - 1] = tmp;
        }
    }
}

void pylt_obj_list_sort(PyLiteInterpreter *I, PyLiteListObject *self) {
    // TODO
}

PyLiteObject* pylt_obj_list_pop(PyLiteInterpreter *I, PyLiteListObject *self) {
    if (self->ob_size == 0) return NULL;
    return self->ob_val[--self->ob_size];
}

PyLiteObject* pylt_obj_list_getitem(PyLiteInterpreter *I, PyLiteListObject *self, int index) {
    index_chk(index, NULL);
    return self->ob_val[index];
}

pl_bool_t pylt_obj_list_setitem(PyLiteInterpreter *I, PyLiteListObject *self, int index, PyLiteObject* obj) {
    index_chk(index, false);
    self->ob_val[index] = obj;
    return true;
}

pl_bool_t pylt_obj_list_delitem(PyLiteInterpreter *I, PyLiteListObject *self, int index) {
    index_chk(index, false);
    if (index != self->ob_size - 1) {
        memcpy(self->ob_val + index, self->ob_val + index + 1, (self->ob_size - index - 1) * sizeof(PyLiteObject*));
    }
    self->ob_size--;
    return true;
}

PyLiteListObject* pylt_obj_list_slice(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step) {
    pl_int_t start, end;
    if (step == 0) return NULL;
    start = pstart ? *pstart : (step > 0) ? 0 : self->ob_size;
    end = pend ? *pend : (step > 0) ? self->ob_size : 0;

    index_fix(start);
    index_fix(end);

    if (step < 0) {
        start -= 1;
        if (!pend) end -= 1;
    }

    pl_int_t count = (pl_int_t)ceil(abs(end - start) / (float)abs(step));
    PyLiteListObject *lst = pylt_obj_list_new_with_size(I, count);
    lst->ob_size = count;

    if (step == 1) {
        memcpy(lst->ob_val, self->ob_val + start, count * sizeof(PyLiteObject*));
    } else {
        pl_int_t cur_index = start;
        for (pl_int_t i = 0; i < count; ++i) {
            lst->ob_val[i] = self->ob_val[cur_index];
            cur_index += step;
        }
    }
    return lst;
}

pl_int_t pylt_obj_list_slice_set(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step, PyLiteObject *val) {
    pl_int_t start, end;
    if (step == 0) return -1;
    start = pstart ? *pstart : (step > 0) ? 0 : self->ob_size;
    end = pend ? *pend : (step > 0) ? self->ob_size : 0;

    index_fix(start);
    index_fix(end);

    if (step < 0) {
        start -= 1;
        if (!pend) end -= 1;
    }

    PyLiteListObject *lst = NULL;
    pl_int_t count = (pl_int_t)ceil(abs(end - start) / (float)abs(step));
    pl_int_t val_count = pylt_obj_len(I, val);

    if (val_count == -1) {
        lst = pylt_obj_list_new(I);
        PyLiteIterObject *iter = pylt_obj_iter_new(I, val);
        for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            pylt_obj_list_append(I, lst, obj);
        }
        val_count = lst->ob_size;
    }

    if (step != 1 && count != val_count) return -2;
    if (!lst) {
        lst = pylt_obj_list_new_with_size(I, val_count);
        PyLiteIterObject *iter = pylt_obj_iter_new(I, val);
        for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            pylt_obj_list_append(I, lst, obj);
        }
    }

    if (step == 1) {
        pl_int_t newsize = self->ob_size - count + val_count;
        if (count < val_count) {
            list_resize_maxsize_more_then(I, self, newsize);
        }
        memmove(self->ob_val + start + val_count, self->ob_val + start + count, (self->ob_size - count) * sizeof(PyLiteObject*));
        memcpy(self->ob_val + start, lst->ob_val, val_count * sizeof(PyLiteObject*));
        self->ob_size = newsize;
    } else {
        pl_int_t cur_index = start;
        for (pl_int_t i = 0; i < count; ++i) {
            self->ob_val[cur_index] = lst->ob_val[i];
            cur_index += step;
        }
    }
    pylt_obj_list_free(I, lst);
    return 0;
}


void pylt_obj_list_slice_del(PyLiteInterpreter *I, PyLiteListObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step, PyLiteObject *val) {
    pl_int_t start, end;
    if (step == 0) return;
    start = pstart ? *pstart : (step > 0) ? 0 : self->ob_size;
    end = pend ? *pend : (step > 0) ? self->ob_size : 0;

    index_fix(start);
    index_fix(end);

    if (step < 0) {
        start -= 1;
        if (!pend) end -= 1;
    }

    pl_int_t count = (pl_int_t)ceil(abs(end - start) / (float)abs(step));
    if (!count) return;

    if (step == 1) {
        if (end != self->ob_size) {
            memmove(self->ob_val + start, self->ob_val + end, count * sizeof(PyLiteObject*));
        }
    } else {
        pl_int_t cur_index = start;
        for (pl_int_t i = 0; i < count; ++i) {
            self->ob_val[cur_index] = NULL;
            cur_index += step;
        }
        pl_int_t real_index = -1;
        for (pl_int_t i = 0; i < self->ob_size; ++i) {
            if (self->ob_val[i]) {
                self->ob_val[++real_index] = self->ob_val[i];
            }
        }
    }
    self->ob_size = self->ob_size - count;
}


pl_bool_t pylt_obj_list_has(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *obj) {
    return pylt_obj_list_index(I, self, obj) != -1;
}

struct PyLiteStrObject* pylt_obj_list_to_str(PyLiteInterpreter *I, PyLiteListObject *self) {
    int index = 0;
    PyLiteStrObject *str;
    PyLiteStrObject **strlst = NULL;
    pl_uint_t llen = pylt_obj_list_count(I, self);

    if (llen == 0) {
        return pl_static.str.TMPL_EMPTY_LIST; // []
    }

    pl_uint32_t *data;
    pl_uint32_t comma_num = llen - 1;
    pl_uint32_t data_len = 2 + comma_num * 2; // [] + ', '
    strlst = pylt_malloc(I, llen * sizeof(PyLiteStrObject*));

    for (pl_uint_t i = 0; i < llen; ++i) {
        str = pylt_obj_to_repr(I, self->ob_val[i]);
        data_len += str->ob_size;
        strlst[index++] = str;
    }

    data = pylt_malloc(I, data_len * sizeof(uint32_t));
    data[0] = '[';
    index = 1;
    for (pl_uint_t i = 0; i < llen; ++i) {
        memcpy(data + index, strlst[i]->ob_val, strlst[i]->ob_size * sizeof(uint32_t));
        index += strlst[i]->ob_size;
        if (i != llen - 1) {
            data[index++] = ',';
            data[index++] = ' ';
        }
    }
    data[data_len - 1] = ']';

    str = pylt_obj_str_new(I, data, data_len, true);
    pylt_free(I, data, sizeof(uint32_t) * data_len);
    pylt_free_ex(I, strlst);
    return str;
}
