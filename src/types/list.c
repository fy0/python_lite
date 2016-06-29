
#include "list.h"
#include "string.h"
#include "../state.h"

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
	obj->ob_val = (size) ? pylt_realloc(NULL, size * sizeof(PyLiteObject*)) : NULL;
    return obj;
}

void pylt_obj_list_free(PyLiteState *state, PyLiteListObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}

pl_int_t pylt_obj_list_append(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    if (self->ob_size >= self->ob_maxsize) {
		// sometimes new with size 0
		if (self->ob_maxsize == 0) self->ob_maxsize = 4;
		else self->ob_maxsize *= 2;
        self->ob_val = pylt_realloc(self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*));
    }
    self->ob_val[self->ob_size++] = obj;
    return self->ob_size - 1;
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

pl_uint_t pylt_obj_list_count(PyLiteState *state, PyLiteListObject *self) {
    return self->ob_size;
}

void pylt_obj_list_extend(PyLiteState *state, PyLiteListObject *self, PyLiteListObject *obj) {
    if (self->ob_size + obj->ob_size >= self->ob_maxsize) {
        self->ob_maxsize = self->ob_size + obj->ob_size;
        self->ob_val = pylt_realloc(self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*));
    }
    /*for (int i = 0; i < obj->ob_size; ++i) {
        self->ob_val[self->ob_size + i] = obj->ob_val[i];
    }*/
    memcpy(self->ob_val + self->ob_size, obj->ob_val, obj->ob_size * sizeof(PyLiteObject*));
    self->ob_size = self->ob_size + obj->ob_size;
}

pl_int_t pylt_obj_list_index(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_eq(state, self->ob_val[i], obj)) return i;
    }
    return -1;
}

// 同时要求类型相同，例如 1 == 1.0 但仍有区别
pl_int_t pylt_obj_list_index_strict(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if ((self->ob_val[i]->ob_type == obj->ob_type) && (pylt_obj_eq(state, self->ob_val[i], obj))) {
            return i;
        }
    }
    return -1;
}

pl_bool_t pylt_obj_list_insert(PyLiteState *state, PyLiteListObject *self, pl_int_t index, PyLiteObject *obj) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return false;

    if (self->ob_size + 1 >= self->ob_maxsize) {
        self->ob_maxsize = self->ob_size + 1;
        self->ob_val = pylt_realloc(self->ob_val, self->ob_maxsize * sizeof(PyLiteObject*));
    }

    memcpy(self->ob_val + index + 1, self->ob_val + index, (self->ob_size - index + 1) * sizeof(PyLiteObject*));
    self->ob_val[index] = obj;
    return true;
}

pl_bool_t pylt_obj_list_remove(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    for (pl_int_t i = 0; i < self->ob_size; ++i) {
        if (pylt_obj_eq(state, self->ob_val[i], obj)) {
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

PyLiteObject* pylt_obj_list_pop(PyLiteState *state, PyLiteListObject *self) {
    if (self->ob_size == 0) return NULL;
    return self->ob_val[--self->ob_size];
}

PyLiteObject* pylt_obj_list_getitem(PyLiteState *state, PyLiteListObject *self, int index) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return NULL;
    return self->ob_val[index];
}

pl_bool_t pylt_obj_list_setitem(PyLiteState *state, PyLiteListObject *self, int index, PyLiteObject* obj) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return false;
    self->ob_val[index] = obj;
    return true;
}

pl_bool_t pylt_obj_list_has(PyLiteState *state, PyLiteListObject *self, PyLiteObject *obj) {
    return pylt_obj_list_index(state, self, obj) != -1;
}

struct PyLiteStrObject* pylt_obj_list_to_str(PyLiteState *state, PyLiteListObject *self) {
	int index = 0;
	PyLiteStrObject *str;
	PyLiteStrObject **strlst = NULL;
	pl_uint_t llen = pylt_obj_list_count(state, self);

	if (llen == 0) {
		return pl_static.str.TMPL_EMPTY_LIST; // []
	}

	pl_uint32_t *data;
	pl_uint32_t comma_num = llen - 1;
	pl_uint32_t data_len = 2 + comma_num * 2; // [] + ', '
	strlst = realloc(NULL, llen * sizeof(PyLiteStrObject*));

	for (pl_uint_t i = 0; i < llen; ++i) {
		str = pylt_obj_to_repr(state, self->ob_val[i]);
		data_len += str->ob_size;
		strlst[index++] = str;
	}

	data = pylt_realloc(NULL, data_len * sizeof(uint32_t));
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

	str = pylt_obj_str_new(state, data, data_len, true);
	pylt_free(data);
	pylt_free(strlst);
	return str;
}
