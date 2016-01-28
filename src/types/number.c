
#include "bool.h"
#include "number.h"

uint32_t pylt_obj_int_hash(PyLiteIntObject *obj) {
    return obj->ob_val;
}

uint32_t pylt_obj_int_eq(PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        return self->ob_val == castint(other)->ob_val;
    case PYLT_OBJ_TYPE_FLOAT:
        return self->ob_val == castfloat(other)->ob_val;
    case PYLT_OBJ_TYPE_BOOL:
        return self->ob_val == castbool(other)->ob_val;
    default:
        return 0;
    }
}

uint32_t pylt_obj_int_cmp(PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        if (self->ob_val < castint(other)->ob_val) return -1;
        if (self->ob_val > castint(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_FLOAT:
        if (self->ob_val < castfloat(other)->ob_val) return -1;
        if (self->ob_val > castfloat(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_BOOL:
        if (self->ob_val < castbool(other)->ob_val) return -1;
        if (self->ob_val > castbool(other)->ob_val) return 1;
        else return 0;
    default:
        return -2;
    }
}

uint32_t pylt_obj_float_hash(PyLiteFloatObject *obj) {
    double num;
    double _frac = modf(obj->ob_val, &num);
    if (_frac == 0) return (uint32_t)num;

    register size_t hash = 0;
    for (int i = 0; i < sizeof(double) / sizeof(uint8_t); i++) {
        hash = hash * 131 + ((uint8_t*)&obj->ob_val)[i];
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t pylt_obj_float_eq(PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        return self->ob_val == castint(other)->ob_val;
    case PYLT_OBJ_TYPE_FLOAT:
        return self->ob_val == castfloat(other)->ob_val;
    case PYLT_OBJ_TYPE_BOOL:
        return self->ob_val == castbool(other)->ob_val;
    default:
        return 0;
    }
}

uint32_t pylt_obj_float_cmp(PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        if (self->ob_val < castint(other)->ob_val) return -1;
        if (self->ob_val > castint(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_FLOAT:
        if (self->ob_val < castfloat(other)->ob_val) return -1;
        if (self->ob_val > castfloat(other)->ob_val) return 1;
        else return 0;
    case PYLT_OBJ_TYPE_BOOL:
        if (self->ob_val < castbool(other)->ob_val) return -1;
        if (self->ob_val > castbool(other)->ob_val) return 1;
        else return 0;
    default:
        return -2;
    }
}

PyLiteObject* pylt_obj_number_from_stream(StringStream *ss) {
    return NULL;
}
