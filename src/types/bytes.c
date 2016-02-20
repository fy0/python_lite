
#include "bytes.h"

uint32_t pylt_obj_bytes_hash(PyLiteState *state, PyLiteBytesObject *obj) {
    return castbytes(obj)->ob_hash;
}

// BKDR Hash
uint32_t pylt_obj_bytes_forcehash(PyLiteState *state, PyLiteBytesObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

uint32_t pylt_obj_bytes_eq(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_BYTES:
        if (self->ob_hash != castbytes(other)->ob_hash)
            return false;
    default:
        return 0;
    }
}


uint32_t pylt_obj_bytes_cmp(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other) {
    return false;
}


PyLiteBytesObject* pylt_obj_bytes_new(PyLiteState *state, const char* str, int size, bool is_raw) {
    PyLiteBytesObject *obj = pylt_realloc(NULL, sizeof(PyLiteBytesObject*));
    obj->ob_type = PYLT_OBJ_TYPE_BYTES;
    if (is_raw) {
        obj->ob_val = pylt_realloc(NULL, sizeof(uint8_t)*size + 1);
        obj->ob_val[size] = '\0';
        memcpy(obj->ob_val, str, size);
    }
    obj->ob_hash = pylt_obj_bytes_forcehash(state, obj);
    return obj;
}
