
#include "unusual.h"
#include "string.h"

PyLiteUnusualObject PyLiteUnknown = { PYLT_OBJ_TYPE_UNUSUAL };

PyLiteUnusualObject PyLiteParamArgs = { PYLT_OBJ_TYPE_UNUSUAL };
PyLiteUnusualObject PyLiteParamKwargs = { PYLT_OBJ_TYPE_UNUSUAL };
PyLiteUnusualObject PyLiteParamUndefined = { PYLT_OBJ_TYPE_UNUSUAL };


struct PyLiteStrObject* pylt_obj_unusual_to_str(PyLiteInterpreter *I, PyLiteUnusualObject *self) {
	if (self == &PyLiteUnknown) return pylt_obj_str_new_from_cstr(I, "<Unknown>", true);
	if (self == &PyLiteParamArgs) return pylt_obj_str_new_from_cstr(I, "<ParamArgs>", true);
	if (self == &PyLiteParamKwargs) return pylt_obj_str_new_from_cstr(I, "<ParamKwargs>", true);
	if (self == &PyLiteParamUndefined) return pylt_obj_str_new_from_cstr(I, "<ParamUndefined>", true);
	return NULL;
}
