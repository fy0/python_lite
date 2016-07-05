
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../config.h"
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

    PYLT_OBJ_TYPE_BASE_EXCEPTION, // 20

    PYLT_OBJ_TYPE_USERCLASS, // 21
};

#define PYLT_OBJ_BUILTIN_TYPE_NUM PYLT_OBJ_TYPE_BASE_EXCEPTION


// Object methods

pl_int_t pylt_obj_cmp(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_bool_t pylt_obj_eq(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);
pl_uint32_t pylt_obj_hash(PyLiteState *state, PyLiteObject *obj);
pl_int_t pylt_obj_len(PyLiteState *state, PyLiteObject *obj);

pl_bool_t pylt_obj_hashable(PyLiteState *state, PyLiteObject *obj);
pl_bool_t pylt_obj_iterable(PyLiteState *state, PyLiteObject *obj);
pl_bool_t pylt_obj_istrue(PyLiteState *state, PyLiteObject *obj);

PyLiteObject* pylt_obj_getattr(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key, pl_bool_t *p_at_type);
pl_bool_t pylt_obj_setattr(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);

PyLiteObject* pylt_obj_getitem(PyLiteState *state, PyLiteObject *obj, PyLiteObject* key);
pl_bool_t pylt_obj_setitem(PyLiteState *state, PyLiteObject *self, PyLiteObject* key, PyLiteObject* value);
//PyLiteObject* pylt_obj_slice(PyLiteState *state, PyLiteObject *obj);

pl_bool_t pylt_obj_has(PyLiteState *state, PyLiteObject *self, PyLiteObject *obj, pl_bool_t *is_valid);

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
struct PyLiteBaseExceptionObject;

struct PyLiteCustomObject;


#define cast(t, exp)    ((t)(exp))
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
#define castcode(i)     cast(struct PyLiteCodeObject*, (i))

#define casttype(i)     cast(struct PyLiteTypeObject*, (i))
#define castiter(i)     cast(struct PyLiteIterObject*, (i))
#define castprop(i)     cast(struct PyLitePropertyObject*, (i))
#define castnone(i)     cast(struct PyLiteNoneObject*, (i))

#define castrange(i)    cast(struct PyLiteRangeObject*, (i))
#define castexcept(i)   cast(struct PyLiteBaseExceptionObject*, (i))

#define castcustom(i)   cast(struct PyLiteCustomObject*, (i))

#define dcast(type, i)  cast##type(pylt_obj_getbase(i))

#define pl_isint(i)        (i->ob_type == PYLT_OBJ_TYPE_INT)
#define pl_isflt(i)        (i->ob_type == PYLT_OBJ_TYPE_FLOAT)
#define pl_isstr(i)        (i->ob_type == PYLT_OBJ_TYPE_STR)
#define pl_isbytes(i)      (i->ob_type == PYLT_OBJ_TYPE_BYTES)
#define pl_isiter(i)       (i->ob_type == PYLT_OBJ_TYPE_ITER)
#define pl_isnum(i)        ((i->ob_type == PYLT_OBJ_TYPE_INT) || (i->ob_type == PYLT_OBJ_TYPE_FLOAT))
#define pl_isstrkind(i)    ((i->ob_type == PYLT_OBJ_TYPE_STR) || (i->ob_type == PYLT_OBJ_TYPE_BYTES))
#define pl_iscustom(i)     (i->ob_type >= PYLT_OBJ_TYPE_USERCLASS)

PyLiteObject* pylt_obj_typecast(PyLiteState *state, struct PyLiteTypeObject *type, PyLiteObject *obj);
PyLiteObject* pylt_obj_getbase(PyLiteObject *obj);
struct PyLiteStrObject* pylt_obj_to_str(PyLiteState *state, PyLiteObject *obj);
struct PyLiteStrObject* pylt_obj_to_repr(PyLiteState *state, PyLiteObject *obj);

void pylt_obj_free(PyLiteState *state, PyLiteObject *obj);
void pylt_obj_safefree(PyLiteState *state, PyLiteObject *obj);

// Others

typedef PyLiteObject* (*PyLiteObjUnaryOpFunc)(PyLiteState *state, PyLiteObject *obj);
typedef PyLiteObject* (*PyLiteObjBinaryOpFunc)(PyLiteState *state, PyLiteObject *a, PyLiteObject *b);

typedef PyLiteObject* (*PyLiteCFunctionPtr)(PyLiteState *state, int argc, PyLiteObject **args);
typedef PyLiteObject* (*PyLiteIterFunc)(PyLiteState *state, struct PyLiteIterObject *iter);

#endif
