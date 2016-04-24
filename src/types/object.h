
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../utils.h"
#include "../io.h"
#include "../lib/khash_obj.h"

// Immutable object

typedef struct PyLiteObject {
    uint32_t ob_type;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type

// Custom object

typedef struct PyLiteCustomObject {
    uint32_t ob_type;
    uint32_t ob_base;
    struct PyLiteTable *ob_attrs;
} PyLiteCustomObject;

#define PyLiteCustomObject_HEAD \
    uint32_t ob_type; \
    uint32_t ob_base; \
    struct PyLiteTable *ob_attrs

enum PyLiteObjectTypeCode {
    PTLT_OBJ_TYPE_OBJ = 1,
    PYLT_OBJ_TYPE_INT,
    PYLT_OBJ_TYPE_FLOAT,
    PYLT_OBJ_TYPE_BOOL,

    PYLT_OBJ_TYPE_STR,  // 5
    PYLT_OBJ_TYPE_BYTES,
    PYLT_OBJ_TYPE_SET,
    PYLT_OBJ_TYPE_LIST,
    PYLT_OBJ_TYPE_TUPLE,
    PYLT_OBJ_TYPE_DICT,

    PYLT_OBJ_TYPE_MODULE, // 11
    PYLT_OBJ_TYPE_FUNCTION,
    PYLT_OBJ_TYPE_CFUNCTION,
    PYLT_OBJ_TYPE_CODESNIPPET,

    PYLT_OBJ_TYPE_TYPE, // 15
    PYLT_OBJ_TYPE_ITER,
    PYLT_OBJ_TYPE_CLASS,
    PYLT_OBJ_TYPE_NONE,
};

#define PYLT_OBJ_TYPE_NUM PYLT_OBJ_TYPE_NONE


// Object methods

pl_int_t pylt_obj_ccmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_bool_t pylt_obj_ceq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_uint32_t pylt_obj_chash(PyLiteState *state, PyLiteObject *obj);

pl_bool_t pylt_obj_chashable(PyLiteState *state, PyLiteObject *obj);
pl_bool_t pylt_obj_citerable(PyLiteState *state, PyLiteObject *obj);
pl_bool_t pylt_obj_cistrue(PyLiteState *state, PyLiteObject *obj);

PyLiteObject* pylt_obj_getattr(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type);
pl_bool_t pylt_obj_setattr(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);

PyLiteObject* pylt_obj_getitem(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key);
pl_bool_t pylt_obj_setitem(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);
//PyLiteObject* pylt_obj_cslice(PyLiteState *state, PyLiteObject *obj);

PyLiteObject* pylt_obj_op_unary(PyLiteState *state, int op, PyLiteObject *obj);
PyLiteObject* pylt_obj_op_binary(PyLiteState *state, int op, PyLiteObject *a, PyLiteObject *b);
//pylt_buildin_isinstance(PyLiteState *state, PyLiteObject *obj, PyLiteTypeObject *type);


// Pylite table
KHASHO_INIT(table, PyLiteObject*, PyLiteObject*, 1, pylt_obj_chash, pylt_obj_ceq);
typedef khasho_t(table) PyLiteTable;

#define pylt_obj_table_new(state) kho_init(table, state)
void pylt_obj_table_set(PyLiteTable *tab, PyLiteObject *key, PyLiteObject *val);
PyLiteObject* pylt_obj_table_get(PyLiteTable *tab, PyLiteObject *key);
bool pylt_obj_table_exists(PyLiteTable *tab, PyLiteObject *key);
bool pylt_obj_table_remove(PyLiteTable *tab, PyLiteObject *key);
int pylt_obj_table_len(PyLiteTable *tab);

pl_int_t pylt_obj_table_begin(PyLiteTable *tab);
pl_int_t pylt_obj_table_end(PyLiteTable *tab);
void pylt_obj_table_next(PyLiteTable *tab, pl_int_t *k);
PyLiteObject* pylt_obj_table_itemkey(PyLiteTable *tab, pl_int_t k);
PyLiteObject* pylt_obj_table_itemvalue(PyLiteTable *tab, pl_int_t k);
void pylt_obj_table_keyvalue(PyLiteTable *tab, pl_int_t k, PyLiteObject **pkey, PyLiteObject **pval);

// Type cast

struct PyLiteIntObject;
struct PyLiteFloatObject;
struct PyLiteBoolObject;

struct PyLiteBytesObject;
struct PyLiteStrObject;
struct PyLiteSetObject;
struct PyLiteListObject;
struct PyLiteTupleObject;
struct PyLiteDictObject;

struct PyLiteModuleObject;
struct PyLiteFunctionObject;
struct PyLiteCFunctionObject;

struct PyLiteTypeObject;
struct PyLiteIterObject;

#define cast(t, exp)	((t)(exp))
#define castobj(i)      cast(struct PyLiteObject*, (i))
#define castint(i)      cast(struct PyLiteIntObject*, (i))
#define castfloat(i)    cast(struct PyLiteFloatObject*, (i))
#define castbool(i)     cast(struct PyLiteBoolObject*, (i))

#define castbytes(i)    cast(struct PyLiteBytesObject*, (i))
#define caststr(i)      cast(struct PyLiteStrObject*, (i))
#define castset(i)      cast(struct PyLiteSetObject*, (i))
#define castlist(i)     cast(struct PyLiteListObject*, (i))
#define casttuple(i)    cast(struct PyLiteTupleObject*, (i))
#define castdict(i)     cast(struct PyLiteDictObject*, (i))

#define castmod(i)      cast(struct PyLiteModuleObject*, (i))
#define castfunc(i)     cast(struct PyLiteFunctionObject*, (i))
#define castcfunc(i)    cast(struct PyLiteCFunctionObject*, (i))

#define casttype(i)     cast(struct PyLiteTypeObject*, (i))
#define castclass(i)    cast(struct PyLiteCustomObject*, (i))
#define castiter(i)     cast(struct PyLiteIterObject*, (i))


void pylt_obj_free(PyLiteObject *obj);
//comparable


// Others

typedef PyLiteObject* (*PyLiteObjUnaryOpFunc)(PyLiteState *state, PyLiteObject *obj);
typedef PyLiteObject* (*PyLiteObjBinaryOpFunc)(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);

typedef PyLiteObject* (*PyLiteCFunctionPtr)(PyLiteState *state, int argc, PyLiteObject **args);
typedef void(*PyLiteCMethod)(PyLiteObject *self, PyLiteObject **args);

typedef PyLiteObject* (*PyLiteIterFunc)(PyLiteState *state, struct PyLiteIterObject *iter);

#endif
