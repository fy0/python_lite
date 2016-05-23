
#include "custom.h"
#include "type.h"
#include "../api.h"

PyLiteObject* pylt_obj_cutstom_new(PyLiteState *state, uint32_t ob_type) {
    PyLiteCustomObject *obj = pylt_realloc(NULL, sizeof(PyLiteCustomObject));
    obj->ob_type = ob_type;
    obj->base_obj = NULL;
    obj->ob_attrs = pylt_obj_dict_new(state);
    return castobj(obj);
}

PyLiteObject* pylt_obj_custom_getattr(PyLiteState *state, PyLiteCustomObject *self, PyLiteObject *key, pl_bool_t *p_at_type) {
    PyLiteObject *obj = pylt_obj_dict_getitem(state, self->ob_attrs, key);
    if (!obj) {
        if (p_at_type) *p_at_type = true;
        return pylt_obj_type_getattr(state, pylt_api_gettype(state, self->ob_type), key, NULL);
    }
    if (p_at_type) *p_at_type = false;
    return obj;
}

void pylt_obj_custom_setattr(PyLiteState *state, PyLiteCustomObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_setitem(state, self->ob_attrs, key, value);
}
