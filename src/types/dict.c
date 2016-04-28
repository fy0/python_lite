
#include "dict.h"

pl_int_t pylt_obj_dict_ccmp(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other) {
    return 2;
}

pl_bool_t pylt_obj_dict_ceq(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other) {
    PyLiteDictObject *a, *b;

    if (other->ob_type == PYLT_OBJ_TYPE_DICT) {
        a = self;
        b = castdict(other);

        if (pylt_obj_table_len(a->ob_val) != pylt_obj_table_len(b->ob_val))
            return false;

        for (pl_int_t it = pylt_obj_table_begin(a->ob_val); it != pylt_obj_table_end(a->ob_val); pylt_obj_table_next(a->ob_val, &it)) {
            PyLiteObject *_a, *_b;
            _a = pylt_obj_table_get(b->ob_val, pylt_obj_table_itemkey(a->ob_val, it));
            _b = pylt_obj_table_itemvalue(a->ob_val, it);
            if (!pylt_obj_ceq(state, _a, _b)) return false;
        }
        return true;
    }
    return false;
}

pl_int_t pylt_obj_dict_len(PyLiteState *state, PyLiteDictObject *self) {
    return pylt_obj_table_len(self->ob_val);
}

PyLiteObject* pylt_obj_dict_cgetitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    return pylt_obj_table_get(self->ob_val, key);
}

void pylt_obj_dict_csetitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_table_set(self->ob_val, key, value);
}

PyLiteObject* pylt_obj_dict_has(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    return pylt_obj_table_get(self->ob_val, key);
}

PyLiteObject* pylt_obj_dict_pop(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    PyLiteObject *obj = pylt_obj_table_get(self->ob_val, key);
    if (obj) {
        pylt_obj_table_remove(self->ob_val, key);
        return obj;
    }
    return NULL;
}

pl_int_t pylt_obj_dict_begin(PyLiteState *state, PyLiteDictObject *self) {
    return pylt_obj_table_begin(self->ob_val);
}

pl_int_t pylt_obj_dict_end(PyLiteState *state, PyLiteDictObject *self) {
    return pylt_obj_table_end(self->ob_val);
}

void pylt_obj_dict_next(PyLiteState *state, PyLiteDictObject *self, pl_int_t *k) {
    pylt_obj_table_next(self->ob_val, k);
}

PyLiteObject* pylt_obj_dict_itemkey(PyLiteState *state, PyLiteDictObject *self, pl_int_t k) {
    return pylt_obj_table_itemkey(self->ob_val, k);
}

PyLiteObject* pylt_obj_dict_itemvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k) {
    return pylt_obj_table_itemvalue(self->ob_val, k);
}

void pylt_obj_dict_keyvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k, PyLiteObject **pkey, PyLiteObject **pval) {
    pylt_obj_table_keyvalue(self->ob_val, k, pkey, pval);
}

PyLiteDictObject* pylt_obj_dict_copy(PyLiteState *state, PyLiteDictObject *self) {
    PyLiteDictObject *obj = pylt_obj_dict_new(state);
    kho_resize(table, obj->ob_val, pylt_obj_dict_len(state, self));

    for (pl_int_t k = pylt_obj_dict_begin(state, self); k != pylt_obj_dict_end(state, self); pylt_obj_dict_next(state, self, &k)) {
        pylt_obj_dict_csetitem(state, obj, pylt_obj_dict_itemkey(state, self, k), pylt_obj_dict_itemvalue(state, self, k));
    }

    return obj;
}

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state) {
    PyLiteDictObject *obj = pylt_realloc(NULL, sizeof(PyLiteDictObject));
    obj->ob_type = PYLT_OBJ_TYPE_DICT;
    obj->ob_val = pylt_obj_table_new(state);
    return obj;
}

PyLiteDictObject* pylt_obj_dict_new_with_tab(PyLiteState *state, PyLiteTable *tab) {
    PyLiteDictObject *obj = pylt_realloc(NULL, sizeof(PyLiteDictObject));
    obj->ob_type = PYLT_OBJ_TYPE_DICT;
    obj->ob_val = tab;
    return obj;
}
