
#include "bool.h"
#include "number.h"
#include "bytes.h"
#include "string.h"
#include "../../deps/fpconv/fpconv.h"

pl_int_t pylt_obj_int_cmp(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
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

pl_bool_t pylt_obj_int_eq(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_int_hash(PyLiteInterpreter *I, PyLiteIntObject *obj) {
    return obj->ob_val;
}

PyLiteObject* pylt_obj_int_bitor(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(I, self->ob_val | castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitxor(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(I, self->ob_val ^ castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitand(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(I, self->ob_val & castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitls(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(I, self->ob_val << castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_bitrs(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT: return castobj(pylt_obj_int_new(I, self->ob_val >> castint(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_plus(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, self->ob_val + castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val + castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_minus(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, self->ob_val - castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val - castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_mul(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, self->ob_val * castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val * castfloat(other)->ob_val));
        case PYLT_OBJ_TYPE_BYTES:
            return pylt_obj_bytes_mul(I, castbytes(other), castobj(self));
        case PYLT_OBJ_TYPE_STR:
            return pylt_obj_str_mul(I, caststr(other), castobj(self));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_div(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, self->ob_val / (double)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val / castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_floordiv(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, self->ob_val / (uint32_t)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_int_new(I, self->ob_val / (uint32_t)castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_mod(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, self->ob_val % castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, fmod(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_int_positive(PyLiteInterpreter *I, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(I, abs(self->ob_val)));
}

PyLiteObject* pylt_obj_int_negative(PyLiteInterpreter *I, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(I, -abs(self->ob_val)));
}

PyLiteObject* pylt_obj_int_bitnot(PyLiteInterpreter *I, PyLiteIntObject *self) {
    return castobj(pylt_obj_int_new(I, ~(self->ob_val)));
}

PyLiteObject* pylt_obj_int_pow(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, (uint32_t)powl(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, pow(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

pl_int_t pylt_obj_float_cmp(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
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

pl_bool_t pylt_obj_float_eq(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_float_hash(PyLiteInterpreter *I, PyLiteFloatObject *obj) {
    double num;
    double _frac = modf(obj->ob_val, &num);
    if (_frac == 0) return (uint32_t)num;

    register size_t hash = 0;
    for (int i = 0; i < sizeof(double) / sizeof(uint8_t); i++) {
        hash = hash * 131 + ((uint8_t*)&obj->ob_val)[i];
    }

    return (hash & 0x7FFFFFFF);
}

PyLiteObject* pylt_obj_float_plus(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, self->ob_val + castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val + castfloat(other)->ob_val));
        default:
            return NULL;
    }
}

PyLiteObject* pylt_obj_float_minus(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, self->ob_val - castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val - castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_mul(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, self->ob_val * castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val * castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_div(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, self->ob_val / (double)castint(other)->ob_val));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, self->ob_val / castfloat(other)->ob_val));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_floordiv(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_int_new(I, (uint32_t)(self->ob_val / (uint32_t)castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_int_new(I, (uint32_t)(self->ob_val / (uint32_t)castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_mod(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, fmod(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, fmod(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}

PyLiteObject* pylt_obj_float_positive(PyLiteInterpreter *I, PyLiteFloatObject *self) {
    return castobj(pylt_obj_float_new(I, fabs(self->ob_val)));
}

PyLiteObject* pylt_obj_float_negative(PyLiteInterpreter *I, PyLiteFloatObject *self) {
    return castobj(pylt_obj_float_new(I, -fabs(self->ob_val)));
}

PyLiteObject* pylt_obj_float_pow(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return castobj(pylt_obj_float_new(I, pow(self->ob_val, castint(other)->ob_val)));
        case PYLT_OBJ_TYPE_FLOAT:
            return castobj(pylt_obj_float_new(I, pow(self->ob_val, castfloat(other)->ob_val)));
        default: return NULL;
    }
}


PyLiteIntObject* pylt_obj_int_new(PyLiteInterpreter *I, pl_int_t val) {
    PyLiteIntObject *obj = pylt_malloc(I, sizeof(PyLiteIntObject));
    obj->ob_type = PYLT_OBJ_TYPE_INT;
    obj->ob_flags = 0;
    obj->ob_val = val;
    return obj;
}

PyLiteFloatObject* pylt_obj_float_new(PyLiteInterpreter *I, double val) {
    PyLiteFloatObject *obj = pylt_malloc(I, sizeof(PyLiteFloatObject));
    obj->ob_type = PYLT_OBJ_TYPE_FLOAT;
    obj->ob_flags = 0;
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


PyLiteIntObject* pylt_obj_int_new_from_cstr_full(PyLiteInterpreter *I, const char *str, pl_int_t size, pl_int_t base_n) {
    pl_int_t iret;
    RawString rs = { str, str + size };

    switch (base_n) {
        case 0: iret = _read_x_int(&rs, 10, _dec, 0); break;
        case 1: iret = _read_x_int(&rs, 16, _hex, 0); break;
        case 2: iret = _read_x_int(&rs, 2, _bin, 0); break;
        case 3: iret = _read_x_int(&rs, 8, _oct, 0); break;
    }
    return pylt_obj_int_new(I, iret);
}


PyLiteFloatObject* pylt_obj_float_new_from_cstr_full(PyLiteInterpreter *I, const char *str, pl_int_t size, pl_int_t point_pos) {
    double fret;
    pl_int_t iret;
    RawString rs = { str, str + size };

    iret = _read_x_int(&rs, 10, _dec, point_pos);
    fret = _read_float(&rs, point_pos + 1);
    return pylt_obj_float_new(I, fret + iret);
}

uint32_t* pylt_obj_int_to_ucs4(PyLiteInterpreter *I, PyLiteIntObject *self, pl_int_t *plen) {
    pl_int_t len;
    pl_bool_t is_neg;
    uint32_t *str_data;
    pl_int_t pos = 0;
    pl_int_t val = self->ob_val;

    // negative number
    is_neg = (val < 0);
    len = (is_neg) ? 1 : 0;
    val = abs(val);

    if (val == 0) len = 1;
    else {
        while (val > 0) {
            len++;
            val /= 10;
        }
    }

    val = abs(self->ob_val);
    str_data = pylt_malloc(I, len * sizeof(uint32_t));
    if (is_neg) str_data[0] = '-';
    if (val == 0) str_data[0] = '0';
    pos = len - 1;

    while (val > 0) {
        str_data[pos--] = '0' + (val % 10);
        val /= 10;
    }

    if (plen) *plen = len;
    return str_data;
}

PyLiteStrObject* pylt_obj_int_to_str(PyLiteInterpreter *I, PyLiteIntObject *self) {
    pl_int_t len;
    PyLiteStrObject *ret;
    uint32_t *str_data;

    str_data = pylt_obj_int_to_ucs4(I, self, &len);
    ret = pylt_obj_str_new(I, str_data, len, true);
    pylt_free(I, str_data, sizeof(uint32_t) * len);
    return ret;
}


uint32_t* pylt_obj_float_to_ucs4(PyLiteInterpreter *I, PyLiteFloatObject *self, pl_int_t *plen) {
    char buf[24 + 1];
    pl_int_t len = fpconv_dtoa(self->ob_val, buf);
    uint32_t *str_data = pylt_malloc(I, len * sizeof(uint32_t));
    for (pl_int_t i = 0; i < len; ++i) {
        str_data[i] = buf[i];
    }
    if (plen) *plen = len;
    return str_data;
}

PyLiteStrObject* pylt_obj_float_to_str(PyLiteInterpreter *I, PyLiteFloatObject *self) {
    pl_int_t len;
    PyLiteStrObject *ret;
    uint32_t *str_data;

    str_data = pylt_obj_float_to_ucs4(I, self, &len);
    ret = pylt_obj_str_new(I, str_data, len, true);
    pylt_free(I, str_data, len*sizeof(uint32_t));
    return ret;
}


void pylt_obj_int_free(PyLiteInterpreter *I, PyLiteIntObject *self) {
    pylt_free_ex(I, self);
}

void pylt_obj_float_free(PyLiteInterpreter *I, PyLiteFloatObject *self) {
    pylt_free_ex(I, self);
}
