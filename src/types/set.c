
#include "set.h"
#include "../debug.h"
#include "../state.h"

pl_int_t pylt_obj_set_cmp(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other) {
    if (other->ob_type == PYLT_OBJ_TYPE_SET) {
        if (pylt_obj_set_eq(state, self, other)) return 0;
        if (pylt_obj_set_contains(state, self, castset(other))) return  1;
        if (pylt_obj_set_contains(state, castset(other), self)) return -1;
        return 3;
    }
    return 2;
}


pl_bool_t pylt_obj_set_eq(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other) {
    if (other->ob_type == PYLT_OBJ_TYPE_SET) {
        if (kho_size(self->ob_val) != kho_size(castset(other)->ob_val))
            return false;

        for (khiter_t it = kho_begin(other->ob_val); it < kho_end(castset(other)->ob_val); ++it) {
            if (!kho_exist(castset(other)->ob_val, it)) continue;
            if (pylt_obj_set_has(state, self, kho_key(castset(other)->ob_val, it)) == NULL) return false;
        }
        return true;
    }
    return false;
}

pl_int_t pylt_obj_set_len(PyLiteState *state, PyLiteSetObject *self) {
    return (pl_int_t)kho_size(self->ob_val);
}

pl_bool_t pylt_obj_set_contains(PyLiteState *state, PyLiteSetObject *self, PyLiteSetObject *other) {
    if (kho_size(self->ob_val) <= kho_size(other->ob_val))
        return false;

    for (khiter_t it = kho_begin(other->ob_val); it < kho_end(other->ob_val); ++it) {
        if (!kho_exist(other->ob_val, it)) continue;
        if (pylt_obj_set_has(state, self, kho_key(other->ob_val, it)) == NULL) return false;
    }
    return true;
}

pl_int_t pylt_obj_set_add(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj) {
    int ret;
    if (!pylt_obj_hashable(state, obj)) return -1;
    kho_put(set_obj, self->ob_val, obj, &ret);
    return 0;
}

void pylt_obj_set_clear(PyLiteState *state, PyLiteSetObject *self) {
    kho_clear_set_obj(self->ob_val);
}

PyLiteSetObject* pylt_obj_set_copy(PyLiteState *state, PyLiteSetObject *self) {
    PyLiteSetObject *obj = pylt_obj_set_new(state);
    kho_resize(set_obj, obj->ob_val, pylt_obj_set_len(state, self));

    for (pl_int_t k = pylt_obj_set_begin(state, self); k != pylt_obj_set_end(state, self); pylt_obj_set_next(state, self, &k)) {
        pylt_obj_set_add(state, obj, pylt_obj_set_itemvalue(state, self, k));
    }

    return obj;
}

PyLiteObject* pylt_obj_set_has(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj) {
    khiter_t x = kho_get(set_obj, self->ob_val, obj);
    return (x != kho_end(self->ob_val)) ? kho_key(self->ob_val, x) : NULL;
}

pl_int_t pylt_obj_set_remove(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj) {
    khiter_t x = kho_get(set_obj, self->ob_val, obj);
    if (kho_exist(self->ob_val, x) != 1) return -1;
    kho_del(set_obj, self->ob_val, x);
    return 0;
}

PyLiteObject* pylt_obj_set_pop(PyLiteState *state, PyLiteSetObject *self) {
    khiter_t k;
    if (kho_size(self->ob_val) == 0)
        return NULL;
    k = pylt_obj_set_begin(state, self);
    kho_del(set_obj, self->ob_val, k);
    return pylt_obj_set_itemvalue(state, self, k);
}

pl_int_t pylt_obj_set_begin(PyLiteState *state, PyLiteSetObject *self) {
    pl_int_t k = kho_begin(self->ob_val);
    while (k != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, k)) return k;
        ++k;
    }
    return kho_end(self->ob_val);
}

pl_int_t pylt_obj_set_end(PyLiteState *state, PyLiteSetObject *self) {
    return kho_end(self->ob_val);
}

PyLiteObject* pylt_obj_set_itemvalue(PyLiteState *state, PyLiteSetObject *self, pl_int_t k) {
    return (kho_exist(self->ob_val, k)) ? castobj(kho_key(self->ob_val, k)) : NULL;
}

void pylt_obj_set_next(PyLiteState *state, PyLiteSetObject *self, pl_int_t *k) {
    pl_int_t key = *k;
    while (++key != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, key)) {
            *k = key;
            return;
        }
    }
    *k = kho_end(self->ob_val);
}

struct PyLiteStrObject* pylt_obj_set_to_str(PyLiteState *state, PyLiteSetObject *self) {
	int index = 0;
	PyLiteStrObject *str;
	PyLiteStrObject **strlst = NULL;
	pl_uint_t slen = pylt_obj_set_len(state, self);

	if (slen == 0) {
		return pl_static.str.TMPL_EMPTY_SET; // {}
	}

	pl_uint32_t *data;
	pl_uint32_t comma_num = slen - 1;
	pl_uint32_t data_len = 2 + comma_num * 2; // {} + ', '
	strlst = realloc(NULL, slen * sizeof(PyLiteStrObject*));

    for (pl_int_t k = pylt_obj_set_begin(state, self); k != pylt_obj_set_end(state, self); pylt_obj_set_next(state, self, &k)) {
		str = pylt_obj_to_repr(state, pylt_obj_set_itemvalue(state, self, k));
		data_len += str->ob_size;
		strlst[index++] = str;
    }

	data = pylt_realloc(NULL, data_len * sizeof(uint32_t));
	data[0] = '{';
	index = 1;
	for (pl_uint_t i = 0; i < slen; ++i) {
		memcpy(data + index, strlst[i]->ob_val, strlst[i]->ob_size * sizeof(uint32_t));
		index += strlst[i]->ob_size;
		if (i != slen - 1) {
			data[index++] = ',';
			data[index++] = ' ';
		}
	}
	data[data_len - 1] = '}';

	str = pylt_obj_str_new(state, data, data_len, true);
	pylt_free(data);
	pylt_free(strlst);
	return str;
}

PyLiteSetObject* pylt_obj_set_new(PyLiteState *state) {
    PyLiteSetObject *obj = pylt_realloc(NULL, sizeof(PyLiteSetObject));
    obj->ob_type = PYLT_OBJ_TYPE_SET;
    obj->ob_val = kho_init(set_obj, state);
    return obj;
}

void pylt_obj_set_free(PyLiteState *state, PyLiteSetObject* self) {
    kho_destroy(set_obj, self->ob_val);
    pylt_free(self);
}
