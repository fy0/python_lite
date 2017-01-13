
#include "custom.h"
#include "common/type.h"
#include "../api.h"

PyLiteObject* pylt_obj_cutstom_create(PyLiteInterpreter *I, uint32_t ob_type, PyLiteObject *base_obj) {
    PyLiteCustomObject *obj = pylt_malloc(I, sizeof(PyLiteCustomObject));

    obj->ob_type = ob_type;
    obj->ob_flags = 0;
    obj->ob_attrs = pylt_obj_dict_new(I);
    obj->base_obj = base_obj;

    return castobj(obj);
}

PyLiteObject* pylt_obj_custom_getattr(PyLiteInterpreter *I, PyLiteCustomObject *self, PyLiteObject *key, pl_bool_t *p_at_type) {
    PyLiteObject *obj = pylt_obj_dict_getitem(I, self->ob_attrs, key);
    if (!obj) {
        if (p_at_type) *p_at_type = true;
        PyLiteTypeObject *type = pl_type_by_code(I, self->ob_type);
        while (true) {
            obj = pylt_obj_dict_getitem(I, type->ob_attrs, key);
            if (obj) return obj;
            if (type->ob_reftype == PYLT_OBJ_TYPE_OBJ) break;
            type = pl_type_by_code(I, type->ob_base);
        }
    }
    if (p_at_type) *p_at_type = false;
    return obj;
}

void pylt_obj_custom_setattr(PyLiteInterpreter *I, PyLiteCustomObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_setitem(I, self->ob_attrs, key, value);
}

pl_bool_t pylt_obj_custom_delattr(PyLiteInterpreter *I, PyLiteCustomObject *self, PyLiteObject* key) {
    return pylt_obj_dict_remove(I, self->ob_attrs, key);
}
