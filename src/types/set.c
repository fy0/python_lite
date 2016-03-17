
#include "set.h"
#include "../debug.h"

pl_int_t pylt_obj_set_ccmp(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other) {
    if (other->ob_type == PYLT_OBJ_TYPE_SET) {
        if (pylt_obj_set_ceq(state, self, other)) return 0;
        if (pylt_obj_set_contains(state, self, castset(other))) return  1;
        if (pylt_obj_set_contains(state, castset(other), self)) return -1;
        return 3;
    }
    return 2;
}


pl_bool_t pylt_obj_set_ceq(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other) {
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
    if (!pylt_obj_chashable(state, obj)) return -1;
    kho_put(set_obj, self->ob_val, obj, &ret);
    return 0;
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
    khiter_t x;
    if (kho_size(self->ob_val) == 0)
        return NULL;
    x = kho_begin(self->ob_val);
    if (kho_exist(self->ob_val, x))
        return kho_key(self->ob_val, x);
    return NULL;
}

pl_int_t pylt_obj_set_begin(PyLiteState *state, PyLiteSetObject *self) {
    pl_int_t k = kho_begin(self->ob_val);
    while (k != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, k)) return k;
        ++k;
    }
    return kho_end(self->ob_val);
}

pl_int_t pylt_obj_set_next(PyLiteState *state, PyLiteSetObject *self, pl_int_t k) {
    while (++k != kho_end(self->ob_val)) {
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

PyLiteSetObject* pylt_obj_set_new(PyLiteState *state) {
    PyLiteSetObject *obj = pylt_realloc(NULL, sizeof(PyLiteSetObject));
    obj->ob_type = PYLT_OBJ_TYPE_SET;
    obj->ob_val = kho_init(set_obj, state);
    return obj;
}
