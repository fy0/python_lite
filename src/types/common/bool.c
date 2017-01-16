
#include "bool.h"
#include "number.h"
#include "../../utils/misc.h"

PyLiteBoolObject PyLiteTrue  = { PYLT_OBJ_TYPE_BOOL, PYLT_OBJ_FLAG_STATIC, -1 };
PyLiteBoolObject PyLiteFalse = { PYLT_OBJ_TYPE_BOOL, PYLT_OBJ_FLAG_STATIC, 0 };

pl_int_t pylt_obj_bool_cmp(PyLiteInterpreter *I, PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            if (self->ob_val < castint(other)->ob_val) return -1;
            if (self->ob_val > castint(other)->ob_val) return 1;
            else return 0;
        case PYLT_OBJ_TYPE_FLOAT:
            if (self->ob_val < castfloat(other)->ob_val) return -1;
            if (self->ob_val > castfloat(other)->ob_val) return 1;
            else return 0;
        case PYLT_OBJ_TYPE_BOOL:
            if (self->ob_val < castbool(other)->ob_val) return -1;
            if (self->ob_val > castbool(other)->ob_val) return 1;
            else return 0;
        default:
            return 2;
    }
}

pl_bool_t pylt_obj_bool_eq(PyLiteInterpreter *I, PyLiteBoolObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_INT:
            return self->ob_val == castint(other)->ob_val;
        case PYLT_OBJ_TYPE_FLOAT:
            return self->ob_val == castfloat(other)->ob_val;
        case PYLT_OBJ_TYPE_BOOL:
            return self->ob_val == castbool(other)->ob_val;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_bool_hash(PyLiteInterpreter *I, PyLiteBoolObject *obj) {
    return obj->ob_val;
}

struct PyLiteStrObject* pylt_obj_bool_to_str(PyLiteInterpreter *I, PyLiteBoolObject *self) {
    return (self->ob_val) ? pl_static.str.True : pl_static.str.False;
}

PyLiteBoolObject* pylt_obj_bool_new(PyLiteInterpreter *I, bool val) {
    return (val == false) ? &PyLiteFalse : &PyLiteTrue;
}
