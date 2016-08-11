
#include "unusual.h"
#include "string.h"

PyLiteUnusualObject PyLiteUnknown = { PYLT_OBJ_TYPE_UNUSUAL };
PyLiteUnusualObject PyLiteUseless = { PYLT_OBJ_TYPE_UNUSUAL };

PyLiteUnusualObject PyLiteParamArgs = { PYLT_OBJ_TYPE_UNUSUAL };
PyLiteUnusualObject PyLiteParamKwargs = { PYLT_OBJ_TYPE_UNUSUAL };
PyLiteUnusualObject PyLiteParamUndefined = { PYLT_OBJ_TYPE_UNUSUAL };


struct PyLiteStrObject* pylt_obj_unusual_to_str(PyLiteInterpreter *I, PyLiteUnusualObject *self) {
	if (self == &PyLiteUnknown) return pylt_obj_str_new_from_cstr(I, "<Unknown>", true);
	if (self == &PyLiteUseless) return pylt_obj_str_new_from_cstr(I, "<Useless>", true);
	if (self == &PyLiteParamArgs) return pylt_obj_str_new_from_cstr(I, "<UParamArgs>", true);
	if (self == &PyLiteParamKwargs) return pylt_obj_str_new_from_cstr(I, "<UParamKwargs>", true);
	if (self == &PyLiteParamUndefined) return pylt_obj_str_new_from_cstr(I, "<UParamUndefined>", true);
	return NULL;
}
