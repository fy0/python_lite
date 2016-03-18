
#include "list.h"

PyLiteListObject* pylt_obj_list_new(PyLiteState *state) {
    PyLiteListObject *obj = pylt_realloc(NULL, sizeof(PyLiteListObject));
    obj->ob_type = PYLT_OBJ_TYPE_LIST;
    obj->ob_size = 0;
    obj->ob_maxsize = 4;
    obj->ob_val = pylt_realloc(NULL, 4 * sizeof(PyLiteObject*));
    return obj;
}

PyLiteListObject* pylt_obj_list_new_with_size(PyLiteState *state, pl_uint_t size) {
    PyLiteListObject *obj = pylt_realloc(NULL, sizeof(PyLiteListObject));
    obj->ob_type = PYLT_OBJ_TYPE_LIST;
    obj->ob_size = 0;
    obj->ob_maxsize = size;
    obj->ob_val = pylt_realloc(NULL, size * sizeof(PyLiteObject*));
    return obj;
}

void pylt_obj_list_free(PyLiteState *state, PyLiteListObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}

void pylt_obj_list_append(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    if (self->ob_size >= self->ob_maxsize) {
        self->ob_maxsize *= 2;
        pylt_realloc(self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*));
    }
    self->ob_val[self->ob_size++] = obj;
}

void pylt_obj_list_clear(PyLiteState *state, PyLiteListObject *self) {
    self->ob_size = 0;
}

PyLiteListObject* pylt_obj_list_copy(PyLiteState *state, PyLiteListObject *self) {
    PyLiteListObject *obj = pylt_realloc(NULL, sizeof(PyLiteListObject));
    memcpy(obj, self, sizeof(PyLiteListObject));
    obj->ob_val = pylt_realloc(NULL, obj->ob_size * sizeof(PyLiteObject*));
    memcpy(obj->ob_val, self->ob_val, obj->ob_size * sizeof(PyLiteObject*));
    return obj;
}

pl_uint_t pylt_obj_list_ccount(PyLiteState *state, PyLiteListObject *self) {
    return self->ob_size;
}

void pylt_obj_list_extend(PyLiteState *state, PyLiteListObject *self, PyLiteListObject *obj) {
    if (self->ob_size + obj->ob_size >= self->ob_maxsize) {
        self->ob_maxsize = self->ob_size + obj->ob_size;
        pylt_realloc(self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*));
    }
    memcpy(self->ob_val + self->ob_size, obj->ob_val, obj->ob_size * sizeof(PyLiteObject*));
}

pl_int_t pylt_obj_list_cindex(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_uint_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_ceq(state, self->ob_val[i], obj)) return i;
    }
    return -1;
}

void pylt_obj_list_insert(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    // TODO
}

pl_bool_t pylt_obj_list_remove(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_uint_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_ceq(state, self->ob_val[i], obj)) {
            if (i == self->ob_size - 1) self->ob_size--;
            else {
                memcpy(self->ob_val + i, self->ob_val + i + 1, (self->ob_size - i - 1) * sizeof(PyLiteObject*));
                self->ob_size--;
            }
            return true;
        }
    }
    return false;
}

void pylt_obj_list_reverse(PyLiteState *state, PyLiteListObject *self) {
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

void pylt_obj_list_sort(PyLiteState *state, PyLiteListObject *self) {
    // TODO
}

PyLiteObject* pylt_obj_list_cgetitem(PyLiteState *state, PyLiteListObject *self, int index) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || (pl_uint_t)index >= self->ob_size) return NULL;
    return self->ob_val[index];
}

PyLiteObject* pylt_obj_list_pop(PyLiteState *state, PyLiteListObject *self) {
    if (self->ob_size == 0) return NULL;
    return self->ob_val[--self->ob_size];
}
