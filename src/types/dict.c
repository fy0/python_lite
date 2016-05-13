
#include "dict.h"

pl_int_t pylt_obj_dict_cmp(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other) {
    return 2;
}

pl_bool_t pylt_obj_dict_eq(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *other) {
    PyLiteDictObject *a, *b;

    if (other->ob_type == PYLT_OBJ_TYPE_DICT) {
        a = self;
        b = castdict(other);

        if (pylt_obj_dict_len(state, a) != pylt_obj_dict_len(state, b))
            return false;

        for (pl_int_t it = pylt_obj_dict_begin(state, a); it != pylt_obj_dict_end(state, a); pylt_obj_dict_next(state, a, &it)) {
            PyLiteObject *_a, *_b;
            _a = pylt_obj_dict_getitem(state, b, pylt_obj_dict_itemkey(state, a, it));
            _b = pylt_obj_dict_itemvalue(state, a, it);
            if (!pylt_obj_eq(state, _a, _b)) return false;
        }
        return true;
    }
    return false;
}

pl_int_t pylt_obj_dict_len(PyLiteState *state, PyLiteDictObject *self) {
    return kho_size(self->ob_val);
}

PyLiteObject* pylt_obj_dict_getitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    khiter_t k = kho_get(table, self->ob_val, key);
    return k == kho_end(self->ob_val) ? NULL : kho_value(self->ob_val, k);
}

void pylt_obj_dict_setitem(PyLiteState *state, PyLiteDictObject *self, PyLiteObject* key, PyLiteObject* value) {
    int ret;
    khiter_t k = kho_put(table, self->ob_val, key, &ret);
    kho_value(self->ob_val, k) = value;
}

PyLiteObject* pylt_obj_dict_has(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    return pylt_obj_dict_getitem(state, self, key);
}

pl_bool_t pylt_obj_dict_remove(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    khiter_t k = kho_get(table, self->ob_val, key);
    if (k == kho_end(self->ob_val)) return false;
    else kho_del(table, self->ob_val, k);
    return true;
}

PyLiteObject* pylt_obj_dict_pop(PyLiteState *state, PyLiteDictObject *self, PyLiteObject *key) {
    PyLiteObject *obj = pylt_obj_dict_getitem(state, self, key);
    if (obj) {
        pylt_obj_dict_remove(state, self, key);
        return obj;
    }
    return NULL;
}

pl_int_t pylt_obj_dict_begin(PyLiteState *state, PyLiteDictObject *self) {
    pl_int_t k = kho_begin(self->ob_val);
    while (k != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, k)) return k;
        ++k;
    }
    return kho_end(self->ob_val);
}

pl_int_t pylt_obj_dict_end(PyLiteState *state, PyLiteDictObject *self) {
    return kho_end(self->ob_val);
}

void pylt_obj_dict_next(PyLiteState *state, PyLiteDictObject *self, pl_int_t *k) {
    pl_int_t key = *k;
    while (++key != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, key)) {
            *k = key;
            return;
        }
    }
    *k = kho_end(self->ob_val);
}

PyLiteObject* pylt_obj_dict_itemkey(PyLiteState *state, PyLiteDictObject *self, pl_int_t k) {
    return (kho_exist(self->ob_val, k)) ? castobj(kho_key(self->ob_val, k)) : NULL;
}

PyLiteObject* pylt_obj_dict_itemvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k) {
    return (kho_exist(self->ob_val, k)) ? castobj(kho_val(self->ob_val, k)) : NULL;
}

void pylt_obj_dict_keyvalue(PyLiteState *state, PyLiteDictObject *self, pl_int_t k, PyLiteObject **pkey, PyLiteObject **pval) {
    if (kho_exist(self->ob_val, k)) {
        *pkey = castobj(kho_key(self->ob_val, k));
        *pval = castobj(kho_val(self->ob_val, k));
    } else {
        *pkey = NULL;
        *pval = NULL;
    }
}

PyLiteDictObject* pylt_obj_dict_copy(PyLiteState *state, PyLiteDictObject *self) {
    PyLiteDictObject *obj = pylt_obj_dict_new(state);
    kho_resize(table, obj->ob_val, pylt_obj_dict_len(state, self));

    for (pl_int_t k = pylt_obj_dict_begin(state, self); k != pylt_obj_dict_end(state, self); pylt_obj_dict_next(state, self, &k)) {
        pylt_obj_dict_setitem(state, obj, pylt_obj_dict_itemkey(state, self, k), pylt_obj_dict_itemvalue(state, self, k));
    }

    return obj;
}

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state) {
    PyLiteDictObject *obj = pylt_realloc(NULL, sizeof(PyLiteDictObject));
    obj->ob_type = PYLT_OBJ_TYPE_DICT;
    obj->ob_val = kho_init(table, state);
    return obj;
}
