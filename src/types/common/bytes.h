
#ifndef PYLITE_TYPES_BYTES_H
#define PYLITE_TYPES_BYTES_H

#include "../object.h"

typedef struct PyLiteBytesObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint8_t *ob_val;
} PyLiteBytesObject;

pl_int_t pylt_obj_bytes_cmp(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_bytes_eq(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_bytes_hash(PyLiteInterpreter *I, PyLiteBytesObject *obj);
pl_uint32_t pylt_obj_bytes_forcehash(PyLiteInterpreter *I, PyLiteBytesObject *obj);

PyLiteBytesObject* pylt_obj_bytes_getitem(PyLiteInterpreter *I, PyLiteBytesObject *self, int index);

PyLiteObject* pylt_obj_bytes_mul(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_bytes_plus(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteObject *other);

pl_int_t pylt_obj_bytes_index(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteBytesObject *sub);
pl_int_t pylt_obj_bytes_index_full(PyLiteInterpreter *I, PyLiteBytesObject *self, PyLiteBytesObject *sub, pl_int_t start, pl_int_t end);

PyLiteBytesObject* pylt_obj_bytes_slice(PyLiteInterpreter *I, PyLiteBytesObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step);
struct PyLiteStrObject* pylt_obj_bytes_to_str(PyLiteInterpreter *I, PyLiteBytesObject *self);

PyLiteBytesObject* pylt_obj_bytes_new(PyLiteInterpreter *I, const char* str, int size, bool is_raw);
PyLiteBytesObject* pylt_obj_bytes_new_empty(PyLiteInterpreter *I);
void pylt_obj_bytes_free(PyLiteInterpreter *I, PyLiteBytesObject *self);

#endif
