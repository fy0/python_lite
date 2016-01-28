
#include "bytes.h"

// BKDR Hash
uint32_t pylt_obj_bytes_hash(PyLiteBytesObject *obj) {
    register size_t hash = 0;
    for (unsigned int i = 0; i < obj->ob_size; i++) {
        hash = hash * 131 + obj->ob_val[i];
    }
    return (hash & 0x7FFFFFFF);
}

uint32_t pylt_obj_bytes_eq(PyLiteBytesObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
    case PYLT_OBJ_TYPE_BYTES:
        if (self->ob_hash != castbytes(other)->ob_hash)
            return false;
    default:
        return 0;
    }
}


uint32_t pylt_obj_bytes_cmp(PyLiteBytesObject *self, PyLiteObject *other) {
    return false;
}
