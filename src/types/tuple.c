
#include "tuple.h"
#include "string.h"
#include "../state.h"

struct PyLiteStrObject* pylt_obj_tuple_to_str(PyLiteState *state, PyLiteTupleObject *self) {
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
	strlst = realloc(NULL, tlen * sizeof(PyLiteStrObject*));

	for (pl_uint_t i = 0; i < tlen; ++i) {
		str = pylt_obj_to_repr(state, self->ob_val[i]);
		data_len += str->ob_size;
		strlst[index++] = str;
	}

	data = pylt_realloc(NULL, data_len * sizeof(uint32_t));
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

	str = pylt_obj_str_new(state, data, data_len, true);
	pylt_free(data);
	pylt_free(strlst);
	return str;
}

PyLiteTupleObject* pylt_obj_tuple_new(PyLiteState *state, pl_int_t len) {
    PyLiteTupleObject *obj = pylt_realloc(NULL, sizeof(PyLiteTupleObject));
    obj->ob_type = PYLT_OBJ_TYPE_TUPLE;
    obj->ob_val = (len) ? pylt_realloc(NULL, len * sizeof(PyLiteObject*)) : NULL;
    obj->ob_size = len;
    return obj;
}


PyLiteTupleObject* pylt_obj_tuple_new_with_data(PyLiteState *state, pl_int_t len, void *data) {
    PyLiteTupleObject *obj = pylt_obj_tuple_new(state, len);
    if (len) memcpy(obj->ob_val, data, len * sizeof(PyLiteObject*));
    return obj;
}

PyLiteObject* pylt_obj_tuple_getitem(PyLiteState *state, PyLiteTupleObject *self, int index) {
    if (index < 0) index += self->ob_size;
    if (index < 0 || index >= self->ob_size) return NULL;
    return self->ob_val[index];
}

void pylt_obj_tuple_free(PyLiteState *state, PyLiteTupleObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}
