
#include "type.h"
#include "dict.h"

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, pl_uint32_t instance_type, pl_uint32_t base_type) {
    PyLiteTypeObject *obj = pylt_realloc(NULL, sizeof(PyLiteTypeObject));
    obj->ob_type = PYLT_OBJ_TYPE_TYPE;
    obj->ob_attrs = pylt_obj_dict_new(state);
    obj->ob_reftype = instance_type;
    obj->ob_base = base_type;
    return obj;
}


PyLiteObject* pylt_obj_type_getattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject *key) {
    return pylt_obj_dict_cgetitem(state, self->ob_attrs, key);
}


void pylt_obj_type_setattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_csetitem(state, self->ob_attrs, key, value);
}

pl_uint32_t pylt_obj_type_chash(PyLiteState *state, PyLiteTypeObject *self) {
    return self->ob_reftype;
}
