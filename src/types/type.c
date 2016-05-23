
#include "type.h"
#include "dict.h"
#include "../state.h"
#include "../api.h"

PyLiteTypeObject* pylt_obj_type_new_with_type(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t instance_type, pl_uint32_t base_type) {
    PyLiteTypeObject *obj = pylt_realloc(NULL, sizeof(PyLiteTypeObject));
    obj->name = name;
    obj->ob_type = PYLT_OBJ_TYPE_TYPE;
    obj->ob_attrs = pylt_obj_dict_new(state);
    obj->ob_reftype = instance_type;
    obj->ob_base = base_type;
    return obj;
}

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t base_type, PyLiteDictObject *dict) {
    PyLiteTypeObject *type = pylt_obj_type_new_with_type(state, name, ++state->class_num, base_type);
    type->ob_attrs = (dict) ? pylt_obj_dict_copy(state, dict) : pylt_obj_dict_new(state);
    return type;
}

PyLiteObject* pylt_obj_type_getattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject *key, pl_bool_t *p_at_type) {
    PyLiteObject *obj;
    if (p_at_type) *p_at_type = false;

    while (true) {
        obj = pylt_obj_dict_getitem(state, self->ob_attrs, key);
        if (obj) return obj;
        if (self->ob_reftype == PYLT_OBJ_TYPE_OBJ) break;
        self = pylt_api_gettype(state, self->ob_base);
        if (p_at_type) *p_at_type = true;
    }

    if (self->ob_reftype != PYLT_OBJ_TYPE_TYPE) {
        obj = pylt_obj_dict_getitem(state, pylt_api_gettype(state, PYLT_OBJ_TYPE_TYPE)->ob_attrs, key);
        if (p_at_type) *p_at_type = true;
        if (obj) return obj;
    }

    return NULL;
}


void pylt_obj_type_setattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_setitem(state, self->ob_attrs, key, value);
}

pl_uint32_t pylt_obj_type_hash(PyLiteState *state, PyLiteTypeObject *self) {
    return self->ob_reftype;
}

void pylt_obj_type_free(PyLiteState *state, PyLiteTypeObject *self) {
    pylt_obj_str_free(state, self->name);
    pylt_obj_dict_free(state, self->ob_attrs);
    pylt_free(self);
}
