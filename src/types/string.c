
#include "bool.h"
#include "string.h"
#include "list.h"
#include "../misc.h"
#include "../api.h"

static PyLiteStrObject* hash_and_check_cache(PyLiteState *state, PyLiteStrObject *obj) {
    obj->ob_hash = pylt_obj_str_forcehash(state, obj);
    return (state) ? pylt_gc_cache_str_add(state, obj) : obj;
}

pl_int_t pylt_obj_str_cmp(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    return false;
}

pl_bool_t pylt_obj_str_eq(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    unsigned int i;
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_STR:
            if (self->ob_hash != caststr(other)->ob_hash)
                return false;
            if (self->ob_size != caststr(other)->ob_size)
                return false;
            for (i = 0; i < self->ob_size; i++) {
                if (self->ob_val[i] != caststr(other)->ob_val[i]) return false;
            }
            return true;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_str_hash(PyLiteState *state, PyLiteStrObject *obj) {
    return obj->ob_hash;
}

// BKDR Hash
pl_uint32_t pylt_obj_str_forcehash(PyLiteState *state, PyLiteStrObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

PyLiteStrObject* pylt_obj_str_getitem(PyLiteState *state, PyLiteStrObject *self, int index) {
    uint32_t buf[1];
    int len = self->ob_size;
    if (index < 0) index += len;
    if (index < 0 || index >= len) return NULL;
    else {
        buf[0] = self->ob_val[index];
        return pylt_obj_str_new(state, buf, 1, true);
    }
    return NULL;
}

PyLiteObject* pylt_obj_str_mul(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    PyLiteStrObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_INT) return NULL;

    obj = pylt_realloc(NULL, sizeof(PyLiteStrObject));
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_size = self->ob_size * castint(other)->ob_val;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint32_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    for (int i = 0; i < castint(other)->ob_val; ++i) {
        memcpy(obj->ob_val + i * self->ob_size, self->ob_val, self->ob_size * sizeof(uint32_t));
    }

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(state, obj));
}

PyLiteObject* pylt_obj_str_plus(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other) {
    PyLiteStrObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_STR) return NULL;

    obj = pylt_realloc(NULL, sizeof(PyLiteStrObject));
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_size = self->ob_size + caststr(other)->ob_size;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint32_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    memcpy(obj->ob_val, self->ob_val, self->ob_size * sizeof(uint32_t));
    memcpy(obj->ob_val + self->ob_size, caststr(other)->ob_val, caststr(other)->ob_size * sizeof(uint32_t));

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(state, obj));
}

#define _isnumber(c) (c >= '0' && c <= '9')
#define _ishex(c) (_isnumber(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

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

_INLINE static
int _read_x_int(uint32_t *p, int *pnum, uint8_t(*func)(uint32_t code), int max_size) {
    uint32_t *e = p + max_size;
    int ret = 0, num = 0, val = (int)pow(10, e - p - 1);

    do {
        ret += (*func)(*p++) * val;
        val /= 10;
        num++;
    } while (p != e);

    *pnum = num;
    return ret;
}

typedef struct {
    PyLiteStrObject *fstr; // format str
    pl_uint32_t findex;

    PyLiteStrObject *dstr; // dest str
    pl_uint32_t dindex;
    pl_uint32_t dsize;
} str_writer_t;

pl_int_t str_escape_next(PyLiteState *state, str_writer_t *w) {
    int num;
    PyLiteStrObject *str = w->dstr;
    PyLiteStrObject *format = w->fstr;
    uint32_t format_size = format->ob_size;

    switch (format->ob_val[w->findex]) {
        case '\\':
            if (++(w->findex) >= format_size) goto _def;
            switch (format->ob_val[w->findex]) {
                case 'a': str->ob_val[w->dindex++] = 7; w->findex++; break;
                case 'b': str->ob_val[w->dindex++] = 8; w->findex++; break;
                case 'f': str->ob_val[w->dindex++] = 12; w->findex++; break;
                case 'n': str->ob_val[w->dindex++] = 10; w->findex++; break;
                case 'r': str->ob_val[w->dindex++] = 13; w->findex++; break;
                case 't': str->ob_val[w->dindex++] = 9; w->findex++; break;
                case 'v': str->ob_val[w->dindex++] = 11; break;
                case '\\': str->ob_val[w->dindex++] = '\\'; break;
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                    str->ob_val[w->dindex++] = _read_x_int(format->ob_val + w->findex, &num, _oct, min(format_size - w->findex, 3));
                    w->findex += num;
                    break;
                case 'x':
                    w->findex++;
                    if ((format_size - w->findex >= 2) && (_ishex(format->ob_val[w->findex]) && _ishex(format->ob_val[w->findex + 1]))) {
                        str->ob_val[w->dindex++] = _hex(format->ob_val[w->findex]) * 16 + _hex(format->ob_val[w->findex + 1]);
                    } else {
                        pylt_obj_str_free(state, str);
                        // error: not hex
                        return -1;
                    }
                    w->findex += 2;
                    break;
                default:
                    if (format->ob_val[++w->findex] >= 0x80) {
                        // error
                        return -2;
                    }
                    str->ob_val[w->dindex++] = '\\';
                    str->ob_val[w->dindex++] = format->ob_val[w->findex];
                    break;
            }
        default: _def :
            str->ob_val[w->dindex++] = format->ob_val[w->findex++];
    }
    return 0;
}

PyLiteStrObject* pylt_obj_str_new(PyLiteState *state, uint32_t *str, int size, bool is_raw) {
    PyLiteStrObject *obj = pylt_realloc(NULL, sizeof(PyLiteStrObject));
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_val = pylt_realloc(NULL, sizeof(uint32_t) * (size + 1));
    if (is_raw) {
        obj->ob_size = size;
        for (int i = 0; i < size; i++) {
            obj->ob_val[i] = str[i];
        }
        obj->ob_val[size] = '\0';
    } else {
        int pos = 0, num;
        for (int i = 0; i < size;) {
            switch (str[i]) {
                case '\\':
                    if (++i >= size) goto _def;
                    switch (str[i]) {
                        case 'a': obj->ob_val[pos++] = 7; i++; break;
                        case 'b': obj->ob_val[pos++] = 8; i++; break;
                        case 'f': obj->ob_val[pos++] = 12; i++; break;
                        case 'n': obj->ob_val[pos++] = 10; i++; break;
                        case 'r': obj->ob_val[pos++] = 13; i++; break;
                        case 't': obj->ob_val[pos++] = 9; i++; break;
                        case 'v': obj->ob_val[pos++] = 11; break;
                        case '\\': obj->ob_val[pos++] = '\\'; break;
                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                            obj->ob_val[pos++] = _read_x_int(str + i, &num, _oct, min(size - i, 3));
                            i += num;
                            break;
                        case 'x':
                            i++;
                            if ((size - i >= 2) && (_ishex(str[i]) && _ishex(str[i + 1]))) {
                                obj->ob_val[pos++] = _hex(str[i]) * 16 + _hex(str[i + 1]);
                            } else {
                                pylt_obj_str_free(state, obj);
                                return NULL;
                            } 
                            i += 2;
                            break;
                        default:
                            if (str[++i] >= 0x80) {
                                // error
                                return NULL;
                            }
                            obj->ob_val[pos++] = '\\';
                            obj->ob_val[pos++] = str[i];
                            break;
                    }
                default: _def :
                    obj->ob_val[pos++] = str[i++];
            }
        }
        obj->ob_size = pos;
        obj->ob_val[pos] = '\0';
        obj->ob_val = pylt_realloc(obj->ob_val, sizeof(uint32_t) * (size + 1));
    }
    return hash_and_check_cache(state, obj);
}

PyLiteStrObject* pylt_obj_str_new_from_cstr(PyLiteState *state, const char *str, bool is_raw) {
    uint32_t code;
    PyLiteStrObject *obj;
    const char *p = (const char *)str;
    int len = utf8_len(str);
    uint32_t *buf = pylt_realloc(NULL, (len + 1) * sizeof(uint32_t));

    for (int i = 0; i < len; i++) {
        p = utf8_decode(p, &code);
        if (!p) {
            pylt_free(buf);
            return NULL;
        }
        buf[i] = code;
    }

    obj = pylt_obj_str_new(state, buf, len, is_raw);
    return obj;
}

PyLiteStrObject* pylt_obj_str_new_empty(PyLiteState *state) {
    return pylt_obj_str_new(state, NULL, 0, true);
}

void pylt_obj_str_free(PyLiteState *state, PyLiteStrObject *self) {
    pylt_free(self->ob_val);
    pylt_free(self);
}

void pylt_obj_str_safefree(PyLiteState *state, PyLiteStrObject *self) {
    // TODO: remove
    pylt_obj_str_free(state, self);
}

pl_int_t pylt_obj_str_index_full(PyLiteState *state, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end) {
    pl_int_t i, j, k;
    pl_int_t len_self = (pl_int_t)self->ob_size;

    if (start < 0) start += len_self;
    start = max(0, start);
    if (start >= len_self) return -2;

    if (end < 0) end += len_self;
    end = min(len_self, end);
    if (end <= start) return -2;

    for (i = start; i < end; ++i) {
        if (self->ob_val[i] == sub->ob_val[0]) {
            k = 1;
            j = i + 1;
            while ((j < end) && (k < (pl_int_t)sub->ob_size)) {
                if (self->ob_val[j] != sub->ob_val[k]) {
                    break;
                }
                ++j; ++k;
            }
            if (k == sub->ob_size) {
                return i;
            }
        }
    }
    return -1;
}

pl_int_t pylt_obj_str_index(PyLiteState *state, PyLiteStrObject *self, PyLiteStrObject *sub) {
    return pylt_obj_str_index_full(state, self, sub, 0, self->ob_size);
}

PyLiteStrObject* pylt_obj_str_join(PyLiteState *state, PyLiteStrObject *separator, PyLiteObject *seq) {
    if (pylt_obj_iterable(state, seq)) {
        pl_uint32_t data_len = 0;
        PyLiteIterObject *iter = pylt_obj_iter_new(state, seq);
        PyLiteListObject *strlst = pylt_obj_list_new(state);

        for (PyLiteObject *obj = pylt_obj_iter_next(state, iter); obj; obj = pylt_obj_iter_next(state, iter)) {
            if (!pl_isstr(obj)) {
                pylt_obj_list_free(state, strlst);
                return NULL;
            }
            data_len += caststr(obj)->ob_size;
            pylt_obj_list_append(state, strlst, obj);
        }

        if (strlst->ob_size == 0) {
            pylt_obj_list_free(state, strlst);
            return pl_static.str.TMPL_EMPTY_STR;
        }

        int index = 0;
        pl_uint32_t *data;
        PyLiteStrObject *str;

        data_len += (strlst->ob_size - 1) * separator->ob_size;
        data = pylt_realloc(NULL, data_len * sizeof(uint32_t));

        for (pl_int_t i = 0; i < strlst->ob_size; ++i) {
            str = caststr(strlst->ob_val[i]);
            memcpy(data + index, str->ob_val, str->ob_size * sizeof(uint32_t));
            index += str->ob_size;
            if (i != strlst->ob_size - 1) {
                memcpy(data + index, separator->ob_val, separator->ob_size * sizeof(uint32_t));
                index += separator->ob_size;
            }
        }

        str = pylt_obj_str_new(state, data, data_len, true);
        pylt_free(data);
        pylt_obj_list_free(state, strlst);
        return str;
    }
    return NULL;
}

PyLiteStrObject* pylt_obj_str_new_from_vformat(PyLiteState *state, PyLiteStrObject *format, va_list args) {
    pl_int_t slen;
    uint32_t *tmp;
    PyLiteObject *obj;
    PyLiteStrObject *str = pylt_realloc(NULL, sizeof(PyLiteStrObject));
    str->ob_type = PYLT_OBJ_TYPE_STR;
    str->ob_val = pylt_realloc(NULL, sizeof(uint32_t) * (format->ob_size + 1));

    str_writer_t writer = {
        .fstr = format,
        .findex = 0,

        .dstr = str,
        .dindex = 0,
        .dsize = format->ob_size,
    };

    for (; writer.findex < format->ob_size;) {
        if (format->ob_val[writer.findex] == '%') {
            pl_bool_t is_free = true;
            writer.findex++;
            obj = va_arg(args, PyLiteObject*);

            switch (format->ob_val[writer.findex]) {
                case 'c':
                    // char
                    writer.findex++;
                    if (pl_isint(obj)) {
                        // 0 - 0x110000
                    } else if (pl_isstr(obj)) {
                        ;
                    } else {
                        // Error
                    }
                    break;
                case 'd':
                    writer.findex++;
                    if (pl_isint(obj)) {
                        tmp = pylt_obj_int_to_ucs4(state, castint(obj), &slen);
                    } else {
                        // Error
                    }
                    break;
                case 'f': case 'F': case 'g': case 'G':
                    writer.findex++;
                    if (pl_isflt(obj)) {
                        tmp = pylt_obj_float_to_ucs4(state, castfloat(obj), &slen);
                    } else {
                        // Error
                    }
                    break;
                case 'o':
                    // TODO: oct number
                    writer.findex++;
                    break;
                case 'x': case 'X':
                    // TODO: hex number
                    writer.findex++;
                    break;
                case 's':
                    writer.findex++;
                    if (pl_isstr(obj)) {
                        tmp = caststr(obj)->ob_val;
                        slen = caststr(obj)->ob_size;
                        is_free = false;
                    } else {
                        // Error
                    }
                    break;
                case 'p': {
                    uint8_t tmp_bytes[MAX_LONG_LONG_CHARS];
                    slen = sprintf(tmp_bytes, "%p", obj) + 2;

                    tmp = pylt_realloc(NULL, slen*sizeof(uint32_t));
                    tmp[0] = '0';
                    tmp[1] = 'x';
                    for (int i = 2; i < slen; ++i) {
                        tmp[i] = tmp_bytes[i-2];
                    } 

                    writer.findex++;
                    break;
                }
                default:
                    // ValueError: incomplete format
                    pylt_obj_str_free(state, str);
                    return NULL;
            }

            pl_uint32_t rest = writer.fstr->ob_size - writer.findex;
            if (writer.dindex + rest + slen + 1 > writer.dsize) {
                writer.dstr->ob_val = pylt_realloc(writer.dstr->ob_val, sizeof(uint32_t) * (format->ob_size + rest + slen + 1));
                writer.dsize = format->ob_size + rest + slen;
            }

            memcpy(writer.dstr->ob_val + writer.dindex, tmp, sizeof(uint32_t)*slen);
            writer.dindex += slen;
            if (is_free) pylt_free(tmp);
        } else {
            str_escape_next(state, &writer);
        }
    }
    str->ob_size = writer.dindex;
    str->ob_val[writer.dindex] = '\0';
    //str->ob_val = pylt_realloc(str->ob_val, sizeof(uint32_t) * (str_size + 1));

    return hash_and_check_cache(state, str);
}

PyLiteStrObject* pylt_obj_str_new_from_cstr_static(PyLiteState *state, const char *str, bool is_raw) {
    PyLiteStrObject *ret = pylt_obj_str_new_from_cstr(state, str, is_raw);
    pylt_gc_make_str_static(state, castobj(ret));
    return ret;
}

PyLiteStrObject* pylt_obj_str_new_from_format(PyLiteState *state, PyLiteStrObject *format, ...) {
    va_list args;
    va_start(args, format);
    PyLiteStrObject *str = pylt_obj_str_new_from_vformat(state, format, args);
    va_end(args);
    return str;
}

PyLiteStrObject* pylt_obj_str_new_from_cformat(PyLiteState *state, const char *format, ...) {
    va_list args;
    va_start(args, format);
    PyLiteStrObject *str = pylt_obj_str_new_from_vformat(state, pylt_obj_str_new_from_cstr(state, format, true), args);
    va_end(args);
    return str;
}

PyLiteStrObject* pylt_obj_str_new_from_cformat_static(PyLiteState *state, const char *format, ...) {
    va_list args;
    va_start(args, format);
    PyLiteStrObject *str = pylt_obj_str_new_from_vformat(state, pylt_obj_str_new_from_cstr(state, format, true), args);
    pylt_gc_make_str_static(state, castobj(str));
    va_end(args);
    return str;
}
