
#include "tuple.h"
#include "string.h"

// 将 index 转为标准形式并约束到可用范围
#define index_fix(__index) \
    if (__index < 0) __index += self->ob_size; \
    if (__index < 0) __index = 0; \
        else if (__index >= self->ob_size) __index = self->ob_size-1;


#define index_chk(__index, failret) \
    if (__index < 0) __index += self->ob_size; \
    if (__index < 0 || __index >= self->ob_size) return failret;


struct PyLiteStrObject* pylt_obj_tuple_to_str(PyLiteInterpreter *I, PyLiteTupleObject *self) {
    int index = 0;
    PyLiteStrObject *str;
    PyLiteStrObject **strlst = NULL;
    pl_uint_t tlen = self->ob_size;

    if (tlen == 0) {
        return pl_static.str.TMPL_EMPTY_TUPLE; // ()
    }

    pl_uint32_t *data;
    pl_uint32_t comma_num = tlen - 1;
    pl_uint32_t data_len = 2 + comma_num * 2; // () + ', '
    strlst = pylt_malloc(I, tlen * sizeof(PyLiteStrObject*));

    for (pl_uint_t i = 0; i < tlen; ++i) {
        str = pylt_obj_to_repr(I, self->ob_val[i]);
        data_len += str->ob_size;
        strlst[index++] = str;
    }

    data = pylt_malloc(I, data_len * sizeof(uint32_t));
    data[0] = '(';
    index = 1;
    for (pl_uint_t i = 0; i < tlen; ++i) {
        memcpy(data + index, strlst[i]->ob_val, strlst[i]->ob_size * sizeof(uint32_t));
        index += strlst[i]->ob_size;
        if (i != tlen - 1) {
            data[index++] = ',';
            data[index++] = ' ';
        }
    }
    data[data_len - 1] = ')';

    str = pylt_obj_str_new(I, data, data_len, true);
    pylt_free(I, data, data_len * sizeof(uint32_t));
    pylt_free_ex(I, strlst);
    return str;
}

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteInterpreter *I, pl_int_t len) {
    PyLiteTupleObject *obj = pylt_malloc(I, sizeof(PyLiteTupleObject));
    obj->ob_type = PYLT_OBJ_TYPE_TUPLE;
    obj->ob_flags = 0;
    obj->ob_val = (len) ? pylt_malloc(I, len * sizeof(PyLiteObject*)) : NULL;
    obj->ob_size = len;
    return obj;
}


PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteInterpreter *I, pl_int_t len, void *data) {
    PyLiteTupleObject *obj = pylt_obj_tuple_new(I, len);
    if (len) memcpy(obj->ob_val, data, len * sizeof(PyLiteObject*));
    return obj;
}

PyLiteObject* pylt_obj_tuple_getitem(PyLiteInterpreter *I, PyLiteTupleObject *self, int index) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return NULL;
    return self->ob_val[index];
}

PyLiteTupleObject* pylt_obj_tuple_slice(PyLiteInterpreter *I, PyLiteTupleObject *self, pl_int_t start, pl_int_t end, pl_int_t step) {
    index_fix(start);
    index_fix(end);
    if (step == 0) return NULL;

    pl_int_t count = (pl_int_t)ceil(abs(end - start) / abs(step));
    PyLiteTupleObject *lst = pylt_obj_tuple_new(I, count);

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

void pylt_obj_tuple_free(PyLiteInterpreter *I, PyLiteTupleObject *self) {
    pylt_free(I, self->ob_val, sizeof(PyLiteObject*) * self->ob_size);
    pylt_free_ex(I, self);
}
