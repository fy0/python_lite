
#include "dict.h"
#include "string.h"
#include "../../utils/misc.h"

pl_int_t pylt_obj_dict_cmp(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *other) {
    return 2;
}

pl_bool_t pylt_obj_dict_eq(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *other) {
    PyLiteDictObject *a, *b;

    if (other->ob_type == PYLT_OBJ_TYPE_DICT) {
        a = self;
        b = castdict(other);

        if (pylt_obj_dict_len(I, a) != pylt_obj_dict_len(I, b))
            return false;

        for (pl_int32_t it = pylt_obj_dict_begin(I, a); it != pylt_obj_dict_end(I, a); pylt_obj_dict_next(I, a, &it)) {
            PyLiteObject *_a, *_b;
            _a = pylt_obj_dict_getitem(I, b, pylt_obj_dict_itemkey(I, a, it));
            _b = pylt_obj_dict_itemvalue(I, a, it);
            if (!pylt_obj_eq(I, _a, _b)) return false;
        }
        return true;
    }
    return false;
}

pl_int_t pylt_obj_dict_len(PyLiteInterpreter *I, PyLiteDictObject *self) {
    return kho_size(self->ob_val);
}

PyLiteObject* pylt_obj_dict_getitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    khiter_t k = kho_get(table, self->ob_val, key);
    return k == kho_end(self->ob_val) ? NULL : kho_value(self->ob_val, k);
}

void pylt_obj_dict_setitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject* key, PyLiteObject* value) {
    int ret;
    khiter_t k = kho_put(table, self->ob_val, key, &ret);
    kho_value(self->ob_val, k) = value;
}

pl_bool_t pylt_obj_dict_has(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    return pylt_obj_dict_getitem(I, self, key) != NULL;
}

pl_bool_t pylt_obj_dict_remove(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    khiter_t k = kho_get(table, self->ob_val, key);
    if (k == kho_end(self->ob_val)) return false;
    else kho_del(table, self->ob_val, k);
    return true;
}

PyLiteObject* pylt_obj_dict_pop(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    PyLiteObject *obj = pylt_obj_dict_getitem(I, self, key);
    if (obj) {
        pylt_obj_dict_remove(I, self, key);
        return obj;
    }
    return NULL;
}

pl_int32_t pylt_obj_dict_begin(PyLiteInterpreter *I, PyLiteDictObject *self) {
    pl_int32_t k = kho_begin(self->ob_val);
    while (k != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, k)) return k;
        ++k;
    }
    return kho_end(self->ob_val);
}

pl_int32_t pylt_obj_dict_end(PyLiteInterpreter *I, PyLiteDictObject *self) {
    return kho_end(self->ob_val);
}

void pylt_obj_dict_next(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t *k) {
    pl_int32_t key = *k;
    while (++key != kho_end(self->ob_val)) {
        if (kho_exist(self->ob_val, key)) {
            *k = key;
            return;
        }
    }
    *k = kho_end(self->ob_val);
}

PyLiteObject* pylt_obj_dict_itemkey(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k) {
    return (kho_exist(self->ob_val, k)) ? castobj(kho_key(self->ob_val, k)) : NULL;
}

PyLiteObject* pylt_obj_dict_itemvalue(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k) {
    return (kho_exist(self->ob_val, k)) ? castobj(kho_val(self->ob_val, k)) : NULL;
}

void pylt_obj_dict_keyvalue(PyLiteInterpreter *I, PyLiteDictObject *self, pl_int32_t k, PyLiteObject **pkey, PyLiteObject **pval) {
    if (kho_exist(self->ob_val, k)) {
        *pkey = castobj(kho_key(self->ob_val, k));
        *pval = castobj(kho_val(self->ob_val, k));
    } else {
        *pkey = NULL;
        *pval = NULL;
    }
}

PyLiteDictObject* pylt_obj_dict_copy(PyLiteInterpreter *I, PyLiteDictObject *self) {
    PyLiteDictObject *obj = pylt_obj_dict_new(I);
    kho_resize(table, obj->ob_val, pylt_obj_dict_len(I, self));

    for (pl_int32_t k = pylt_obj_dict_begin(I, self); k != pylt_obj_dict_end(I, self); pylt_obj_dict_next(I, self, &k)) {
        pylt_obj_dict_setitem(I, obj, pylt_obj_dict_itemkey(I, self, k), pylt_obj_dict_itemvalue(I, self, k));
    }

    return obj;
}

struct PyLiteStrObject* pylt_obj_dict_to_str(PyLiteInterpreter *I, PyLiteDictObject *self) {
    int index = 0;
    PyLiteStrObject *str;
    PyLiteStrObject **strlst = NULL;
    pl_uint_t dlen = pylt_obj_dict_len(I, self);

    if (dlen == 0) {
        return pl_static.str.TMPL_EMPTY_DICT; // []
    }

    pl_uint32_t *data;
    pl_uint32_t comma_num = dlen - 1;
    pl_uint32_t data_len = 2 + comma_num * 2 + dlen * 2; // {} + ', ' + ': '
    strlst = pylt_malloc(I, dlen * 2 * sizeof(PyLiteStrObject*));

    for (pl_int32_t k = pylt_obj_dict_begin(I, self); k != pylt_obj_dict_end(I, self); pylt_obj_dict_next(I, self, &k)) {
        str = pylt_obj_to_repr(I, pylt_obj_dict_itemkey(I, self, k));
        data_len += str->ob_size;
        strlst[index++] = str;

        str = pylt_obj_to_repr(I, pylt_obj_dict_itemvalue(I, self, k));
        data_len += str->ob_size;
        strlst[index++] = str;
    }

    data = pylt_malloc(I, data_len * sizeof(uint32_t));
    data[0] = '{';
    index = 1;
    for (pl_uint_t i = 0; i < dlen*2; ++i) {
        // key
        memcpy(data + index, strlst[i]->ob_val, strlst[i]->ob_size * sizeof(uint32_t));
        index += strlst[i]->ob_size;

        // : 
        data[index++] = ':';
        data[index++] = ' ';
        ++i;

        // value
        memcpy(data + index, strlst[i]->ob_val, strlst[i]->ob_size * sizeof(uint32_t));
        index += strlst[i]->ob_size;

        if (i != dlen*2 - 1) {
            data[index++] = ',';
            data[index++] = ' ';
        }
    }
    data[data_len - 1] = '}';

    str = pylt_obj_str_new(I, data, data_len, true);
    pylt_free(I, data, data_len * sizeof(uint32_t));
    pylt_free_ex(I, strlst);
    return str;
}

PyLiteDictObject* pylt_obj_dict_new(PyLiteInterpreter *I) {
    PyLiteDictObject *obj = pylt_malloc(I, sizeof(PyLiteDictObject));
    obj->ob_type = PYLT_OBJ_TYPE_DICT;
    obj->ob_flags = 0;
    obj->ob_val = kho_init(table, state);
    return obj;
}

void pylt_obj_dict_free(PyLiteInterpreter *I, PyLiteDictObject *self) {
    kho_destroy(table, self->ob_val);
    pylt_free_ex(I, self);
}
