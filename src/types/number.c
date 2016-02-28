
#include "bool.h"
#include "number.h"

pl_int_t pylt_obj_int_ccmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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
            if (self->ob_val < (int32_t)castbool(other)->ob_val) return -1;
            if (self->ob_val >(int32_t)castbool(other)->ob_val) return 1;
            else return 0;
        default:
            return 2;
    }
}

pl_bool_t pylt_obj_int_ceq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_int_chash(PyLiteState *state, PyLiteIntObject *obj) {
    return obj->ob_val;
}

PyLiteObject* pylt_obj_int_bitor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(state, self->ob_val | castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitxor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(state, self->ob_val ^ castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitand(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(state, self->ob_val & castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitls(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(state, self->ob_val << castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitrs(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(state, self->ob_val >> castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_plus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val + castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val + castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_minus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val - castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val - castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_mul(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val * castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val * castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_div(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, self->ob_val / (double)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val / castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_floordiv(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val / (uint32_t)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_int_new(state, self->ob_val / (uint32_t)castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_mod(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, self->ob_val % castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, fmod(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_positive(PyLiteState *state, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(state, abs(self->ob_val)));
}

PyLiteObject* pylt_obj_int_negative(PyLiteState *state, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(state, -abs(self->ob_val)));
}

PyLiteObject* pylt_obj_int_bitnot(PyLiteState *state, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(state, ~(self->ob_val)));
}

PyLiteObject* pylt_obj_int_pow(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, (uint32_t)powl(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, pow(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

pl_int_t pylt_obj_float_ccmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
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

pl_bool_t pylt_obj_float_ceq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_INT:
        return self->ob_val == castint(other)->ob_val;
    case PYLT_OBJ_TYPE_FLOAT:
        return self->ob_val == castfloat(other)->ob_val;
    case PYLT_OBJ_TYPE_BOOL:
        return self->ob_val == castbool(other)->ob_val;
    default:
        return false;
    }
}

pl_uint32_t pylt_obj_float_chash(PyLiteState *state, PyLiteFloatObject *obj) {
    double num;
    double _frac = modf(obj->ob_val, &num);
    if (_frac == 0) return (uint32_t)num;

    register size_t hash = 0;
    for (int i = 0; i < sizeof(double) / sizeof(uint8_t); i++) {
        hash = hash * 131 + ((uint8_t*)&obj->ob_val)[i];
    }

    return (hash & 0x7FFFFFFF);
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

PyLiteObject* pylt_obj_float_minus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, self->ob_val - castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val - castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_mul(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, self->ob_val * castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val * castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_div(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, self->ob_val / (double)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, self->ob_val / castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_floordiv(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(state, (uint32_t)(self->ob_val / (uint32_t)castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_int_new(state, (uint32_t)(self->ob_val / (uint32_t)castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_mod(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, fmod(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, fmod(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_positive(PyLiteState *state, PyLiteFloatObject *self) {
    return castobj(pylt_obj_float_new(state, fabs(self->ob_val)));
}

PyLiteObject* pylt_obj_float_negative(PyLiteState *state, PyLiteFloatObject *self) {
    return castobj(pylt_obj_float_new(state, -fabs(self->ob_val)));
}

PyLiteObject* pylt_obj_float_pow(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(state, pow(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(state, pow(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}


PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, int32_t val) {
    PyLiteIntObject *obj = pylt_realloc(NULL, sizeof(PyLiteIntObject));
    obj->ob_type = PYLT_OBJ_TYPE_INT;
    obj->ob_val = val;
    return obj;
}

PyLiteFloatObject* pylt_obj_float_new(PyLiteState *state, double val) {
    PyLiteFloatObject *obj = pylt_realloc(NULL, sizeof(PyLiteFloatObject));
    obj->ob_type = PYLT_OBJ_TYPE_FLOAT;
    obj->ob_val = val;
    return obj;
}
