
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../utils.h"
#include "../io.h"
#include "../lib/khash.h"

// Immutable object

typedef struct PyLiteObject {
    uint32_t ob_type;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type

// Custom object

typedef struct PyLiteCustomObject {
    uint32_t ob_type;
    struct PyLiteTable *ob_attrs;
} PyLiteCustomObject;

#define PyLiteCustomObject_HEAD \
    uint32_t ob_type; \
    struct PyLiteTable *ob_attrs

enum PyLiteObjectTypeCode {
    PYLT_OBJ_TYPE_INT = 1,
    PYLT_OBJ_TYPE_FLOAT,
    PYLT_OBJ_TYPE_BOOL,

    PYLT_OBJ_TYPE_STR,
    PYLT_OBJ_TYPE_BYTES,
    PYLT_OBJ_TYPE_SET,
    PYLT_OBJ_TYPE_LIST,
    PYLT_OBJ_TYPE_TUPLE,
    PYLT_OBJ_TYPE_DICT,

    PYLT_OBJ_TYPE_FUNCTION,
    PYLT_OBJ_TYPE_MODULE,

    PYLT_OBJ_TYPE_TYPE,
    PYLT_OBJ_TYPE_CLASS,
};

// Object methods

uint32_t pylt_obj_hash(PyLiteObject *obj);
uint32_t pylt_obj_op_eq(PyLiteObject *a, PyLiteObject *b);
uint32_t pylt_obj_op_plus(PyLiteObject *a, PyLiteObject *b);

void pylt_obj_free(PyLiteObject *obj);
bool pylt_obj_hashable(PyLiteObject *obj);
//comparable

// Pylite table
KHASH_INIT(table, PyLiteObject*, PyLiteObject*, 1, pylt_obj_hash, pylt_obj_op_eq);
typedef khash_t(table) PyLiteTable;

#define pylt_obj_table_new() kh_init(table);
void pylt_obj_table_set(PyLiteTable *tab, PyLiteObject *key, PyLiteObject *val);
PyLiteObject* pylt_obj_table_get(PyLiteTable *tab, PyLiteObject *key);
bool pylt_obj_table_exists(PyLiteTable *tab, PyLiteObject *key);
bool pylt_obj_table_remove(PyLiteTable *tab, PyLiteObject *key);
int pylt_obj_table_len(PyLiteTable *tab);


// Type cast

struct PyLiteIntObject;
struct PyLiteFloatObject;
struct PyLiteBoolObject;

struct PyLiteBytesObject;
struct PyLiteStrObject;
struct PyLiteTupleObject;
struct PyLiteListObject;
struct PyLiteDictObject;

#define cast(t, exp)	((t)(exp))
#define castobj(i)      cast(struct PyLiteObject*, (i))
#define castint(i)      cast(struct PyLiteIntObject*, (i))
#define castfloat(i)    cast(struct PyLiteFloatObject*, (i))
#define castbool(i)     cast(struct PyLiteBoolObject*, (i))

#define castbytes(i)    cast(struct PyLiteBytesObject*, (i))
#define caststr(i)      cast(struct PyLiteStrObject*, (i))
#define casttuple(i)    cast(struct PyLiteTupleObject*, (i))
#define castlist(i)     cast(struct PyLiteListObject*, (i))
#define castdict(i)     cast(struct PyLiteDictObject*, (i))

#define castclass(i)    cast(struct PyLiteCustomObject*, (i))

// Others
const char* pylt_obj_type_name(int ob_type);

void(*PyLiteCFunction)(PyLiteObject *args);
void(*PyLiteCMethod)(PyLiteObject *self, PyLiteObject *args);

#endif
