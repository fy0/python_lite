
#include "bool.h"
#include "number.h"

uint32_t pylt_obj_bool_hash(PyLiteBoolObject *obj) {
    return obj->ob_val;
}

uint32_t pylt_obj_bool_eq(PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        return self->ob_val == castint(other)->ob_val;
    case PYLT_OBJ_TYPE_BOOL:
        return self->ob_val == castbool(other)->ob_val;
    default:
        return 0;
    }
}

uint32_t pylt_obj_bool_cmp(PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        if (self->ob_val < castint(other)->ob_val) return -1;
        if (self->ob_val > castint(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_BOOL:
        if (self->ob_val < castbool(other)->ob_val) return -1;
        if (self->ob_val > castbool(other)->ob_val) return 1;
        else return 0;
    default:
        return -2;
    }
}
