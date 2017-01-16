
#include "unusual.h"
#include "string.h"

PyLiteUnusualObject PyLiteUseless = { PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_FLAG_STATIC };
PyLiteUnusualObject PyLiteNotImplemented = { PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_FLAG_STATIC };

PyLiteUnusualObject PyLiteParamArgs = { PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_FLAG_STATIC };
PyLiteUnusualObject PyLiteParamKwargs = { PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_FLAG_STATIC };
PyLiteUnusualObject PyLiteParamUndefined = { PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_FLAG_STATIC };


struct PyLiteStrObject* pylt_obj_unusual_to_str(PyLiteInterpreter *I, PyLiteUnusualObject *self) {
    if (self == &PyLiteNotImplemented) return pylt_obj_str_new_from_cstr(I, "NotImplemented", true);
    if (self == &PyLiteUseless) return pylt_obj_str_new_from_cstr(I, "<Useless>", true);
    if (self == &PyLiteParamArgs) return pylt_obj_str_new_from_cstr(I, "<UParamArgs>", true);
    if (self == &PyLiteParamKwargs) return pylt_obj_str_new_from_cstr(I, "<UParamKwargs>", true);
    if (self == &PyLiteParamUndefined) return pylt_obj_str_new_from_cstr(I, "<UParamUndefined>", true);
    return NULL;
}
