
#include "set.h"

void pylt_obj_set_add(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj) {
    int ret;
    kho_put(set_obj, self->ob_val, obj, &ret);
}

PyLiteObject* pylt_obj_set_has(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj) {
    khiter_t x = kho_get(set_obj, self->ob_val, obj);
    return (x != kho_end(self->ob_val)) ? kho_key(self->ob_val, x) : NULL;
}

PyLiteSetObject* pylt_obj_set_new(PyLiteState *state) {
    PyLiteSetObject *obj = pylt_realloc(NULL, sizeof(PyLiteSetObject));
    obj->ob_type = PYLT_OBJ_TYPE_SET;
    obj->ob_val = kho_init(set_obj, state);
    return obj;
}
