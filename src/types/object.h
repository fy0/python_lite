
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../utils.h"

// Pylite table

typedef struct PyLiteTableEntry {
    struct PyLiteObject *key;
    struct PyLiteObject *val;
    struct PyLiteTableEntry *prev;
} PyLiteTableEntry;

typedef struct PyLiteTable {
    int used;
    int size;
    PyLiteTableEntry *data;
} PyLiteTable;

PyLiteTable* pylt_obj_table_new(int size);

// Immutable object

typedef struct PyLiteObject {
    uint32_t ob_type;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type;

// Custom object

typedef struct PyLiteCustomObject {
    uint32_t ob_type;
    struct PyLiteTable *ob_attrs;
} PyLiteCustomObject;

#define PyLiteCustomObject_HEAD \
    uint32_t ob_type; \
    struct PyLiteTable *ob_attrs;

enum PyLiteObjectTypeCode {
    PYLT_OBJ_TYPE_INT = 1,
    PYLT_OBJ_TYPE_FLOAT,
    PYLT_OBJ_TYPE_BOOL,

    PYLT_OBJ_TYPE_STR,
    PYLT_OBJ_TYPE_BYTES,
    PYLT_OBJ_TYPE_TUPLE,
    PYLT_OBJ_TYPE_LIST,
    PYLT_OBJ_TYPE_DICT,

    PYLT_OBJ_TYPE_FUNCTION,
    PYLT_OBJ_TYPE_MODULE,

    PYLT_OBJ_TYPE_TYPE,
};

// Object methods

uint32_t pylt_obj_hash(PyLiteObject *obj);

// Type cast

struct PyLiteIntObject;

#define cast(t, exp)	((t)(exp))
#define castint(i)      cast(struct PyLiteIntObject*, (i))

// Others

void(*PyLiteCFunction)(PyLiteObject *args);
void(*PyLiteCMethod)(PyLiteObject *self, PyLiteObject *args);

#endif
