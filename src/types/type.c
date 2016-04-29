
#include "type.h"
#include "dict.h"
#include "../state.h"
#include "../api.h"

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t instance_type, pl_uint32_t base_type) {
    PyLiteTypeObject *obj = pylt_realloc(NULL, sizeof(PyLiteTypeObject));
    obj->name = name;
    obj->ob_type = PYLT_OBJ_TYPE_TYPE;
    obj->ob_attrs = pylt_obj_dict_new(state);
    obj->ob_reftype = instance_type;
    obj->ob_base = base_type;
    return obj;
}

PyLiteTypeObject* pylt_obj_type_new_with_vars(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t base_type, PyLiteDictObject *dict) {
    PyLiteTypeObject *type = pylt_obj_type_new(state, name, ++state->class_num, base_type);
    type->ob_attrs = pylt_obj_dict_copy(state, dict);
    return type;
}

PyLiteObject* pylt_obj_type_getattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject *key, pl_bool_t *p_at_type) {
    PyLiteObject *obj;

    if (p_at_type) *p_at_type = false;
    while (true) {
        obj = pylt_obj_dict_cgetitem(state, self->ob_attrs, key);
        if (obj) return obj;
        if (self->ob_reftype == PYLT_OBJ_TYPE_OBJ) return NULL;
        self = pylt_api_gettype(state, self->ob_base);
        if (p_at_type) *p_at_type = true;
    }
}


void pylt_obj_type_setattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_csetitem(state, self->ob_attrs, key, value);
}

pl_uint32_t pylt_obj_type_chash(PyLiteState *state, PyLiteTypeObject *self) {
    return self->ob_reftype;
}
