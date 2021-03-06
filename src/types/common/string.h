
#ifndef PYLITE_TYPES_STRING_H
#define PYLITE_TYPES_STRING_H

#include "../object.h"

typedef struct PyLiteStrObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint32_t *ob_val;
} PyLiteStrObject;

pl_int_t pylt_obj_str_cmp(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_str_eq(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_str_hash(PyLiteInterpreter *I, PyLiteStrObject *obj);
pl_uint32_t pylt_obj_str_forcehash(PyLiteInterpreter *I, PyLiteStrObject *obj);

PyLiteStrObject* pylt_obj_str_getitem(PyLiteInterpreter *I, PyLiteStrObject *self, int index);

PyLiteObject* pylt_obj_str_mul(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_str_mod(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_str_plus(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteObject *other);

pl_int_t pylt_obj_str_index(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub);
pl_int_t pylt_obj_str_index_full(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end);
PyLiteStrObject* pylt_obj_str_join(PyLiteInterpreter *I, PyLiteStrObject *separator, PyLiteObject *seq);

PyLiteStrObject* pylt_obj_str_slice(PyLiteInterpreter *I, PyLiteStrObject *self, pl_int_t *pstart, pl_int_t *pend, pl_int_t step);
pl_bool_t pylt_obj_str_startswith(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub);
pl_bool_t pylt_obj_str_startswith_full(PyLiteInterpreter *I, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end);
PyLiteStrObject* pylt_obj_str_to_repr(PyLiteInterpreter *I, PyLiteStrObject *self);

PyLiteStrObject* pylt_obj_str_new(PyLiteInterpreter *I, uint32_t *str, int size, bool is_raw);
PyLiteStrObject* pylt_obj_str_new_from_cstr(PyLiteInterpreter *I, const char *str, bool is_raw);
PyLiteStrObject* pylt_obj_str_new_from_wstr(PyLiteInterpreter *I, const wchar_t *str, bool is_raw);

PyLiteStrObject* pylt_obj_str_new_from_format_with_tuple(PyLiteInterpreter *I, PyLiteStrObject *format, struct PyLiteTupleObject *args);
PyLiteStrObject* pylt_obj_str_new_from_format(PyLiteInterpreter *I, PyLiteStrObject *format, ...);
PyLiteStrObject* pylt_obj_str_new_from_cformat(PyLiteInterpreter *I, const char *format, ...);
PyLiteStrObject* pylt_obj_str_new_empty(PyLiteInterpreter *I);

// dont't free bytes/str object unless you know what will happen
void pylt_obj_str_release(PyLiteInterpreter *I, PyLiteStrObject *self);

#endif
