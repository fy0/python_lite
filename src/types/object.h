
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../misc.h"
#include "../io.h"
#include "../lib/khash_obj.h"

// Immutable object

typedef struct PyLiteObject {
    uint32_t ob_type;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type

enum PyLiteObjectTypeCode {
    PYLT_OBJ_TYPE_OBJ = 1,
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
    PYLT_OBJ_TYPE_CODE,

    PYLT_OBJ_TYPE_TYPE, // 15
    PYLT_OBJ_TYPE_ITER,
    PYLT_OBJ_TYPE_PROP,
    PYLT_OBJ_TYPE_NONE,

    PYLT_OBJ_TYPE_RANGE, // 19

    PYLT_OBJ_TYPE_USERCLASS, // 20
};

#define PYLT_OBJ_BUILTIN_TYPE_NUM PYLT_OBJ_TYPE_NONE


// Object methods

pl_int_t pylt_obj_ccmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_bool_t pylt_obj_ceq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_uint32_t pylt_obj_chash(PyLiteState *state, PyLiteObject *obj);
pl_int_t pylt_obj_len(PyLiteState *state, PyLiteObject *obj);

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

struct PyLiteCodeObject;
struct PyLiteModuleObject;
struct PyLiteFunctionObject;
struct PyLiteCFunctionObject;

struct PyLiteTypeObject;
struct PyLiteIterObject;
struct PyLitePropertyObject;
struct PyLiteNoneObject;

struct PyLiteRangeObject;

struct PyLiteCustomObject;


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
#define castiter(i)     cast(struct PyLiteIterObject*, (i))
#define castprop(i)     cast(struct PyLitePropertyObject*, (i))
#define castnone(i)     cast(struct PyLiteNoneObject*, (i))

#define castrange(i)    cast(struct PyLiteRangeObject*, (i))

#define castcustom(i)    cast(struct PyLiteCustomObject*, (i))


void pylt_obj_free(PyLiteObject *obj);

// Others

typedef PyLiteObject* (*PyLiteObjUnaryOpFunc)(PyLiteState *state, PyLiteObject *obj);
typedef PyLiteObject* (*PyLiteObjBinaryOpFunc)(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);

typedef PyLiteObject* (*PyLiteCFunctionPtr)(PyLiteState *state, int argc, PyLiteObject **args);
typedef PyLiteObject* (*PyLiteIterFunc)(PyLiteState *state, struct PyLiteIterObject *iter);

#endif
