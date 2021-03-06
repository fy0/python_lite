﻿
#include "bool.h"
#include "string.h"
#include "list.h"
#include "../../gc.h"
#include "../../api.h"
#include "../../utils/misc.h"

// 将 index 转为标准形式并约束到可用范围
#define index_fix(obj, __index) \
    if (__index < 0) __index += (obj)->ob_size; \
    if (__index < 0) __index = 0; \
    else if (__index >= (pl_int_t)(obj)->ob_size) __index = (obj)->ob_size;


static PyLiteStrObject* hash_and_check_cache(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    obj->ob_hash = pylt_obj_str_forcehash(I, obj);
    return (I) ? pylt_gc_cache_str_add(I, obj) : obj;
}

pl_int_t pylt_obj_str_cmp(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other) {
    return false;
}

pl_bool_t pylt_obj_str_eq(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other) {
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

pl_uint32_t pylt_obj_str_hash(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    return obj->ob_hash;
}

// BKDR Hash
pl_uint32_t pylt_obj_str_forcehash(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

PyLiteStrObject* pylt_obj_str_getitem(PyLiteInterpreter *I, PyLiteStrObject *self, int index) {
    uint32_t buf[1];
    int len = self->ob_size;
    if (index < 0) index += len;
    if (index < 0 || index >= len) return NULL;
    else {
        buf[0] = self->ob_val[index];
        return pylt_obj_str_new(I, buf, 1, true);
    }
    return NULL;
}

PyLiteObject* pylt_obj_str_mul(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other) {
    PyLiteStrObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_INT) return NULL;

    obj = pylt_malloc(I, sizeof(PyLiteStrObject));
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_size = self->ob_size * castint(other)->ob_val;
    obj->ob_val = pylt_malloc(I, sizeof(uint32_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    for (int i = 0; i < castint(other)->ob_val; ++i) {
        memcpy(obj->ob_val + i * self->ob_size, self->ob_val, self->ob_size * sizeof(uint32_t));
    }

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(I, obj));
}


PyLiteObject* pylt_obj_str_mod(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other) {
    if (pl_istuple(other)) {
        return castobj(pl_formatT(I, self, casttuple(other)));
    } else {
        PyLiteStrObject *ret;
        PyLiteTupleObject *tuple = pylt_obj_tuple_new(I, 1);
        tuple->ob_val[0] = other;
        ret = pl_formatT(I, self, tuple);
        pylt_obj_tuple_free(I, tuple);
        return castobj(ret);
    }
}

PyLiteObject* pylt_obj_str_plus(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other) {
    PyLiteStrObject *obj;
    if (other->ob_type != PYLT_OBJ_TYPE_STR) return NULL;

    obj = pylt_malloc(I, sizeof(PyLiteStrObject));
    obj->ob_type = PYLT_OBJ_TYPE_STR;
    obj->ob_size = self->ob_size + caststr(other)->ob_size;
    obj->ob_val = pylt_malloc(I, sizeof(uint32_t) * (obj->ob_size + 1));
    if (!obj->ob_val); // Memory Error

    memcpy(obj->ob_val, self->ob_val, self->ob_size * sizeof(uint32_t));
    memcpy(obj->ob_val + self->ob_size, caststr(other)->ob_val, caststr(other)->ob_size * sizeof(uint32_t));

    obj->ob_val[obj->ob_size] = '\0';
    return castobj(hash_and_check_cache(I, obj));
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
int _read_x_int(uint32_t *p, int n, uint8_t(*func)(uint32_t code), int *pnum, int max_size) {
    uint32_t *e = p + max_size;
    int ret = 0, num = 0, val = (int)pow(n, e - p - 1);

    do {
        uint8_t c = (*func)(*p++);
        if (c == 0xff) {
            ret = (int)(ret * pow(n, -max_size + num));
            break;
        }
        ret += c * val;
        val /= n;
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

pl_int_t str_escape_next(PyLiteInterpreter *I, str_writer_t *w) {
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
                    str->ob_val[w->dindex++] = _read_x_int(format->ob_val + w->findex, 8, _oct, &num, min(format_size - w->findex, 3));
                    w->findex += num;
                    break;
                case 'x':
                    w->findex++;
                    if ((format_size - w->findex >= 2) && (_ishex(format->ob_val[w->findex]) && _ishex(format->ob_val[w->findex + 1]))) {
                        str->ob_val[w->dindex++] = _hex(format->ob_val[w->findex]) * 16 + _hex(format->ob_val[w->findex + 1]);
                    } else {
                        pylt_obj_str_release(I, str);
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
            break;
        default: _def :
            str->ob_val[w->dindex++] = format->ob_val[w->findex++];
    }
    return 0;
}

PyLiteStrObject* pylt_obj_str_new(PyLiteInterpreter *I, uint32_t *str, int size, bool is_raw) {
    PyLiteObject_init(I, obj, PyLiteStrObject, PYLT_OBJ_TYPE_STR);

    obj->ob_val = pylt_malloc(I, sizeof(uint32_t) * (size + 1));
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
                        case 'v': obj->ob_val[pos++] = 11; i++; break;
                        case '\'': obj->ob_val[pos++] = '\''; i++; break;
                        case '\"': obj->ob_val[pos++] = '\"'; i++; break;
                        case '\\': obj->ob_val[pos++] = '\\'; i++; break;
                        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                            obj->ob_val[pos++] = _read_x_int(str + i, 8, _oct, &num, min(size - i, 3));
                            i += num;
                            break;
                        case 'x':
                            i++;
                            if ((size - i >= 2) && (_ishex(str[i]) && _ishex(str[i + 1]))) {
                                obj->ob_val[pos++] = _hex(str[i]) * 16 + _hex(str[i + 1]);
                            } else {
                                pylt_obj_str_release(I, obj);
                                return NULL;
                            } 
                            i += 2;
                            break;
                        default:
                            if (str[i] >= 0x80) {
                                // error
                                return NULL;
                            }
                            obj->ob_val[pos++] = '\\';
                            obj->ob_val[pos++] = str[i++];
                            break;
                    }
                    break;
                default: _def :
                    obj->ob_val[pos++] = str[i++];
            }
        }
        if (pos != size) {
            obj->ob_val = pylt_realloc(I, obj->ob_val, sizeof(uint32_t) * size, sizeof(uint32_t) * (pos + 1));
        }
        obj->ob_val[pos] = '\0';
        obj->ob_size = pos;
    }
    return hash_and_check_cache(I, obj);
}

PyLiteStrObject* pylt_obj_str_new_from_cstr(PyLiteInterpreter *I, const char *str, bool is_raw) {
    uint32_t code;
    PyLiteStrObject *obj;
    const char *p = (const char *)str;
    int len = uc_ucs4str_from_utf8_size(str, strlen(str));
    uint32_t *buf = pylt_malloc(I, (len + 1) * sizeof(uint32_t));

    for (int i = 0; i < len; i++) {
        int ret = uc_ucs4_from_utf8(p, len - i, &code);
        if (ret <= 0) {
            pylt_free(I, buf, (len + 1) * sizeof(uint32_t));
            return NULL;
        }
        p += ret;
        buf[i] = code;
    }

    obj = pylt_obj_str_new(I, buf, len, is_raw);
    pylt_free(I, buf, (len + 1) * sizeof(uint32_t));
    return obj;
}

PyLiteStrObject* pylt_obj_str_new_from_wstr(PyLiteInterpreter *I, const wchar_t *str, bool is_raw) {
    PyLiteStrObject *obj;
    pl_int_t real_len;
    pl_int_t len = wcslen(str);
    uint32_t *buf = pylt_malloc(I, (len + 1) * sizeof(uint32_t));
    int ret = uc_ucs4str_from_wchar(str, len, buf, &real_len);
    obj = (ret >= 0) ? pylt_obj_str_new(I, buf, real_len, is_raw) : NULL;
    pylt_free(I, buf, (len + 1) * sizeof(uint32_t));
    return obj;
}

PyLiteStrObject* pylt_obj_str_new_empty(PyLiteInterpreter *I) {
    return pylt_obj_str_new(I, NULL, 0, true);
}

pl_int_t pylt_obj_str_index_full(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end) {
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

pl_int_t pylt_obj_str_index(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub) {
    return pylt_obj_str_index_full(I, self, sub, 0, self->ob_size);
}

PyLiteStrObject* pylt_obj_str_join(PyLiteInterpreter *I, PyLiteStrObject *separator, PyLiteObject *seq) {
    if (pylt_obj_iterable(I, seq)) {
        pl_uint32_t data_len = 0;
        PyLiteIterObject *iter = pylt_obj_iter_new(I, seq);
        PyLiteListObject *strlst = pylt_obj_list_new(I);

        for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
            if (!pl_isstr(obj)) {
                pylt_obj_list_free(I, strlst);
                return NULL;
            }
            data_len += caststr(obj)->ob_size;
            pylt_obj_list_append(I, strlst, obj);
        }

        if (strlst->ob_size == 0) {
            pylt_obj_list_free(I, strlst);
            return pl_static.str.TMPL_EMPTY_STR;
        }

        int index = 0;
        pl_uint32_t *data;
        PyLiteStrObject *str;

        data_len += (strlst->ob_size - 1) * separator->ob_size;
        data = pylt_malloc(I, data_len * sizeof(uint32_t));

        for (pl_int_t i = 0; i < strlst->ob_size; ++i) {
            str = caststr(strlst->ob_val[i]);
            memcpy(data + index, str->ob_val, str->ob_size * sizeof(uint32_t));
            index += str->ob_size;
            if (i != strlst->ob_size - 1) {
                memcpy(data + index, separator->ob_val, separator->ob_size * sizeof(uint32_t));
                index += separator->ob_size;
            }
        }

        str = pylt_obj_str_new(I, data, data_len, true);
        pylt_free(I, data, str->ob_size * sizeof(uint32_t));
        pylt_obj_list_free(I, strlst);
        return str;
    }
    return NULL;
}

PyLiteStrObject* pylt_obj_str_slice(PyLiteInterpreter *I, PyLiteStrObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step) {
    pl_int_t start, end;
    if (step == 0) return NULL;
    start = pstart ? *pstart : (step > 0) ? 0 : self->ob_size;
    end = pend ? *pend : (step > 0) ? self->ob_size : 0;

    index_fix(self, start);
    index_fix(self, end);

    if (step < 0) {
        start -= 1;
        if (!pend) end -= 1;
    }

    pl_int_t count = (pl_int_t)ceil(abs(end - start) / abs(step));
    uint32_t *buf = pylt_malloc(I, count * sizeof(uint32_t));

    if (step == 1) {
        memcpy(buf, self->ob_val + start, count * sizeof(uint32_t));
    } else {
        pl_int_t cur_index = start;
        for (pl_int_t i = 0; i < count; ++i) {
            buf[i] = self->ob_val[cur_index];
            cur_index += step;
        }
    }

    PyLiteStrObject *str = pylt_obj_str_new(I, buf, count, true);
    pylt_free(I, buf, count * sizeof(uint32_t));
    return str;
}


pl_bool_t pylt_obj_str_startswith(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub) {
    return pylt_obj_str_startswith_full(I, self, sub, 0, self->ob_size);
}


pl_bool_t pylt_obj_str_startswith_full(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end) {
    index_fix(self, start);
    index_fix(self, end);
    pl_int_t len = end - start;

    if (len < (pl_int_t)sub->ob_size) return false;
    for (pl_uint_t i = 0; i < sub->ob_size; ++i) {
        if (self->ob_val[start + i] != sub->ob_val[i]) return false;
    }
    return true;
}


PyLiteStrObject* pylt_obj_str_to_repr(PyLiteInterpreter *I, PyLiteStrObject *self) {
    pl_uint32_t c;
    pl_uint32_t *data;
    pl_uint_t i, j = 0;
    int old_size = self->ob_size + 2;

    data = pylt_malloc(I, old_size * sizeof(uint32_t));
    data[0] = '\'';
    j = 1;

    for (i = 0; i < self->ob_size; ++i) {
        if ((old_size - j) < 4) {
            data = pylt_realloc(I, data, old_size * sizeof(uint32_t), (old_size + 15) * sizeof(uint32_t));
            old_size += 15;
        }
        switch (self->ob_val[i]) {
            case '\a': data[j++] = '\\'; data[j++] = 'a'; break;
            case '\b': data[j++] = '\\'; data[j++] = 'b'; break;
            case '\f': data[j++] = '\\'; data[j++] = 'f'; break;
            case '\n': data[j++] = '\\'; data[j++] = 'n'; break;
            case '\r': data[j++] = '\\'; data[j++] = 'r'; break;
            case '\t': data[j++] = '\\'; data[j++] = 't'; break;
            case '\v': data[j++] = '\\'; data[j++] = 'v'; break;
            case '\\': data[j++] = '\\'; data[j++] = '\\'; break;
            case '\'': data[j++] = '\\'; data[j++] = '\''; break;
            default: {
                c = self->ob_val[i];
                if ((0 <= c && c <= 32) || (127 <= c && c <= 160)) {
                    data[j++] = '\\';
                    data[j++] = 'x';
                    data[j++] = (c / 16) >= 10 ? (c / 16) - 10 + 'a' : (c / 16) + '0';
                    data[j++] = (c % 16) >= 10 ? (c % 16) - 10 + 'a' : (c % 16) + '0';
                } else {
                    data[j++] = self->ob_val[i];
                }
                break;
            }
        }
    }

    data[j++] = '\'';
    PyLiteStrObject *str = pylt_obj_str_new(I, data, j, true);
    pylt_free(I, data, old_size * sizeof(uint32_t));
    return str;
}

PyLiteStrObject* pylt_obj_str_new_from_format_with_tuple(PyLiteInterpreter *I, PyLiteStrObject *format, PyLiteTupleObject *args) {
    uint32_t *tmp;
    PyLiteObject *obj;
    pl_int_t argtimes = 0;
    pl_int_t slen, rlen; // string length, real length

    PyLiteObject_init(I, str, PyLiteStrObject, PYLT_OBJ_TYPE_STR);
    str->ob_val = pylt_malloc(I, sizeof(uint32_t) * (format->ob_size + 1));

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
            pl_bool_t left_pad = true;
            pl_bool_t pad_with_zero = false;
            pl_int_t cstate = 0; // default state
            pl_int_t limitAll = 0, limitFloat = 0; // "%0.0d"
            uint32_t ch = format->ob_val[++writer.findex];
            obj = pylt_obj_tuple_getitem(I, args, argtimes++);
            if (!obj) {
                // TODO: ERROR
                // format 参数不够了
                return NULL;
            }

            // flag check
            while (true) {
                if (ch == '0') pad_with_zero = true;
                else if (ch == '-') left_pad = false;
                else break;
                ch = format->ob_val[++writer.findex];
            }

            // minimal length
            if (isdigit(ch)) cstate = 1;
            else if (ch == '.') cstate = 2;

            // state 1: num
            if (cstate == 1) {
                while (isdigit(ch)) {
                    limitAll *= 10;
                    limitAll += ch - '0';
                    ch = format->ob_val[++writer.findex];
                }
                if (ch == '.') cstate = 2;
            }

            // state 2: float (dot found)
            if (cstate == 2) {
                ch = format->ob_val[++writer.findex];
                while (isdigit(ch)) {
                    limitFloat *= 10;
                    limitFloat += ch - '0';
                    ch = format->ob_val[++writer.findex];
                }
            }

            switch (format->ob_val[writer.findex]) {
                case 'c':
                    // char
                    writer.findex++;
                    pad_with_zero = false;
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
                        tmp = pylt_obj_int_to_ucs4(I, castint(obj), &slen);
                    } else {
                        // Error
                        // TypeError: %d format: a number is required, not list
                    }
                    break;
                case 'f': case 'F': case 'g': case 'G':
                    writer.findex++;
                    if (pl_isflt(obj)) {
                        tmp = pylt_obj_float_to_ucs4(I, castfloat(obj), &slen);
                        if (limitFloat) {
                            for (pl_int_t i = slen - 1; i >= 0; --i) {
                                if (tmp[i] == '.') {
                                    slen = i + 1 + limitFloat;
                                    break;
                                }
                            }
                        }
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
                case 's': case 'r': {
                    ch = format->ob_val[writer.findex];
                    writer.findex++;
                    pad_with_zero = false;
                    PyLiteStrObject *tmpstr = (ch == 's') ? pylt_obj_to_str(I, obj) : pylt_obj_to_repr(I, obj);
                    tmp = tmpstr->ob_val;
                    slen = tmpstr->ob_size;
                    // wait for gc collect
                    is_free = false;
                    break;
                }
                case 'p': {
                    uint8_t tmp_bytes[MAX_LONG_LONG_CHARS];
#ifdef PLATFORM_WINDOWS
                    slen = sprintf(tmp_bytes, "%p", obj) + 2;
                    tmp = pylt_malloc(I, slen*sizeof(uint32_t));
                    tmp[0] = '0';
                    tmp[1] = 'x';
                    for (int i = 2; i < slen; ++i) {
                        tmp[i] = tmp_bytes[i-2];
                    }
#else
                    slen = sprintf(tmp_bytes, "%p", obj);
                    tmp = pylt_malloc(I, slen*sizeof(uint32_t));
                    for (int i = 0; i < slen; ++i) {
                        tmp[i] = tmp_bytes[i];
                    }
#endif
                    writer.findex++;
                    break;
                }
                default:
                    // ValueError: incomplete format
                    pylt_obj_str_release(I, str);
                    return NULL;
            }

            rlen = (slen > limitAll) ? slen : limitAll;
            pl_int_t pad_len = rlen - slen;

            pl_uint32_t rest = writer.fstr->ob_size - writer.findex;
            if (writer.dindex + rest + rlen > writer.dsize) {
                writer.dstr->ob_val = pylt_realloc(I, writer.dstr->ob_val, sizeof(uint32_t) * (writer.dsize), sizeof(uint32_t) * (writer.dindex + rest + rlen + 1));
                writer.dsize = writer.dindex + rest + rlen;
            }

            if (left_pad && pad_len) {
                ch = pad_with_zero ? '0' : ' ';
                for (pl_int_t i = 0; i < pad_len; ++i) {
                    writer.dstr->ob_val[writer.dindex++] = ch;
                }
            }

            memcpy(writer.dstr->ob_val + writer.dindex, tmp, sizeof(uint32_t)*slen);
            writer.dindex += slen;
            if (is_free) pylt_free(I, tmp, sizeof(uint32_t)*slen);

            if ((!left_pad) && pad_len) {
                for (pl_int_t i = 0; i < pad_len; ++i) {
                    writer.dstr->ob_val[writer.dindex++] = ' ';
                }
            }
        } else {
            str_escape_next(I, &writer);
        }
    }
    str->ob_size = writer.dindex;
    str->ob_val[writer.dindex] = '\0';
    //str->ob_val = pylt_realloc(str->ob_val, sizeof(uint32_t) * (str_size + 1));

    return hash_and_check_cache(I, str);
}

static pl_int_t _get_arg_count_str(PyLiteStrObject *format) {
    pl_int_t args_count = 0;
    if (format->ob_val[0] == '%') args_count++;
    for (pl_uint32_t i = 1; i < format->ob_size; ++i) {
        if (format->ob_val[i] == '%') {
            pl_uint32_t last = format->ob_val[i - 1];
            if (last == '%') args_count--;
            else if (last != (uint32_t)'/') {
                args_count++;
            }
        }
    }
    return args_count;
}

static pl_int_t _get_arg_count_cstr(const char *format) {
    pl_int_t args_count = 0;
    if (format[0] == '\0') return 0;
    if (format[0] == '%') args_count++;
    for (const char *p = format + 1; *p; ++p) {
        if (*p == '%') {
            char last = *(p - 1);
            if (last == '%') args_count--;
            else if (last != '/') {
                args_count++;
            }
        }
    }
    return args_count;
}

PyLiteStrObject* pylt_obj_str_new_from_format(PyLiteInterpreter *I, PyLiteStrObject *format, ...) {
    va_list args;
    pl_int_t args_count = _get_arg_count_str(format);
    PyLiteTupleObject *targs = pylt_obj_tuple_new(I, args_count);

    va_start(args, format);
    for (pl_int_t i = 0; i < args_count; ++i) {
        targs->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);
    PyLiteStrObject *str = pylt_obj_str_new_from_format_with_tuple(I, format, targs);
    pylt_obj_tuple_free(I, targs);
    return str;
}
 
PyLiteStrObject* pylt_obj_str_new_from_cformat(PyLiteInterpreter *I, const char *format, ...) {
    va_list args;
    pl_int_t args_count = _get_arg_count_cstr(format);
    PyLiteTupleObject *targs = pylt_obj_tuple_new(I, args_count);

    va_start(args, format);
    for (pl_int_t i = 0; i < args_count; ++i) {
        targs->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);

    PyLiteStrObject *str = pylt_obj_str_new_from_format_with_tuple(I, pylt_obj_str_new_from_cstr(I, format, true), targs);
    pylt_obj_tuple_free(I, targs);
    return str;
}

void pylt_obj_str_release(PyLiteInterpreter *I, PyLiteStrObject *self) {
    pylt_free(I, self->ob_val, sizeof(uint32_t)*self->ob_size);
    pylt_free_ex(I, self);
}
