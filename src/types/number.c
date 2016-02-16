
#include "bool.h"
#include "number.h"

uint32_t pylt_obj_int_hash(PyLiteState *state, PyLiteIntObject *obj) {
    return obj->ob_val;
}

uint32_t pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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

uint32_t pylt_obj_int_cmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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
            if (self->ob_val < (size_t)castbool(other)->ob_val) return -1;
            if (self->ob_val >(size_t)castbool(other)->ob_val) return 1;
            else return 0;
        default:
            return -2;
    }
}

PyLiteObject* pylt_obj_int_plus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val + castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val + castfloat(other)->ob_val));
        default:
            return NULL;
    }
}
 
uint32_t pylt_obj_float_hash(PyLiteState *state, PyLiteFloatObject *obj) {
    double num;
    double _frac = modf(obj->ob_val, &num);
    if (_frac == 0) return (uint32_t)num;

    register size_t hash = 0;
    for (int i = 0; i < sizeof(double) / sizeof(uint8_t); i++) {
        hash = hash * 131 + ((uint8_t*)&obj->ob_val)[i];
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
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

uint32_t pylt_obj_float_cmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
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

PyLiteObject* pylt_obj_float_plus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, self->ob_val + castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val + castfloat(other)->ob_val));
        default:
            return NULL;
    }
}

PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, uint32_t val) {
    PyLiteIntObject *obj = pylt_realloc(NULL, sizeof(PyLiteIntObject*));
    obj->ob_type = PYLT_OBJ_TYPE_INT;
    obj->ob_val = val;
    return obj;
}

PyLiteFloatObject* pylt_obj_float_new(PyLiteState *state, double val) {
    PyLiteFloatObject *obj = pylt_realloc(NULL, sizeof(PyLiteFloatObject*));
    obj->ob_type = PYLT_OBJ_TYPE_FLOAT;
    obj->ob_val = val;
    return obj;
}
