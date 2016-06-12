
#include "bool.h"
#include "number.h"
#include "bytes.h"
#include "string.h"

pl_int_t pylt_obj_int_cmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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

pl_bool_t pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_int_hash(PyLiteState *state, PyLiteIntObject *obj) {
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
        case PYLT_OBJ_TYPE_BYTES:
            return pylt_obj_bytes_mul(state, castbytes(other), castobj(self));
        case PYLT_OBJ_TYPE_STR:
            return pylt_obj_str_mul(state, caststr(other), castobj(self));
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

pl_int_t pylt_obj_float_cmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
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
            return 2;
    }
}

pl_bool_t pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_float_hash(PyLiteState *state, PyLiteFloatObject *obj) {
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


PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, pl_int_t val) {
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



_INLINE static uint8_t _hex(uint32_t code) {
    if (code >= '0' && code <= '9') return code - '0';
    else if (code >= 'A' && code <= 'F') return code - 'A' + 10;
    else if (code >= 'a' && code <= 'f') return code - 'a' + 10;
    return 255;
}

_INLINE static uint8_t _oct(uint32_t code) {
    if (code >= '0' && code <= '7') return code - '0';
    return 255;
}

_INLINE static uint8_t _bin(uint32_t code) {
    if (code >= '0' && code <= '1') return code - '0';
    return 255;
}

_INLINE static uint8_t _dec(uint32_t code) {
    if (code >= '0' && code <= '9') return code - '0';
    return 255;
}


_INLINE static
pl_int_t _read_x_int(RawString *str, int n, uint8_t(*func)(uint32_t code), int max_size) {
    const uint8_t *p = str->s;
    const uint8_t *e = (max_size > 0) ? str->s + max_size : str->e;
    int ret = 0, val = (int)pow(n, e - p - 1);

    do {
        ret += (*func)(*p++) * val;
        val /= n;
    } while (p != e);

    return ret;
}


_INLINE static
double _read_float(RawString *str, int start_offset) {
    const uint8_t *p = str->s + start_offset;
    const uint8_t *e = str->e;
    double ret = 0, val = 0.1;

    do {
        ret += _dec(*p++) * val;
        val /= 10;
    } while (p != e);

    return ret;
}


PyLiteIntObject* pylt_obj_int_new_from_cstr_full(PyLiteState *state, const char *str, pl_int_t size, pl_int_t base_n) {
    pl_int_t iret;
    RawString rs = { str, str + size };

    switch (base_n) {
        case 0: iret = _read_x_int(&rs, 10, _dec, 0); break;
        case 1: iret = _read_x_int(&rs, 16, _hex, 0); break;
        case 2: iret = _read_x_int(&rs, 2, _bin, 0); break;
        case 3: iret = _read_x_int(&rs, 8, _oct, 0); break;
    }
    return pylt_obj_int_new(state, iret);
}


PyLiteFloatObject* pylt_obj_float_new_from_cstr_full(PyLiteState *state, const char *str, pl_int_t size, pl_int_t point_pos) {
    double fret;
    pl_int_t iret;
    RawString rs = { str, str + size };

    iret = _read_x_int(&rs, 10, _dec, point_pos);
    fret = _read_float(&rs, point_pos + 1);
    return pylt_obj_float_new(state, fret + iret);
}

PyLiteStrObject* pylt_obj_int_to_str(PyLiteState *state, PyLiteIntObject *self) {
    pl_int_t len;
    pl_bool_t is_neg;
    uint32_t *str_data;
    pl_int_t pos = 0;
    pl_int_t val = self->ob_val;
    PyLiteStrObject *ret;
    
    // negative number
    is_neg = (val < 0);
    len = (is_neg) ? 1 : 0;
    val = abs(val);

    while (val > 0) {
        len++;
        val /= 10;
    }

    val = abs(self->ob_val);
    str_data = pylt_realloc(NULL, len * sizeof(uint32_t));
    if (is_neg) str_data[0] = '-';
    pos = len - 1;

    while (val > 0) {
        str_data[pos--] = '0' + (val % 10);
        val /= 10;
    }

    ret = pylt_obj_str_new(state, str_data, len, true);
    pylt_free(str_data);
    return ret;
}

void pylt_obj_int_free(PyLiteState *state, PyLiteIntObject *self) {
    pylt_free(self);
}

void pylt_obj_float_free(PyLiteState *state, PyLiteFloatObject *self) {
    pylt_free(self);
}
