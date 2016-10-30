
#include "module.h"
#include "../common/dict.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteInterpreter *I, PyLiteCodeObject *code, PyLiteStrObject *name) {
	PyLiteModuleObject *obj = castmod(pylt_malloc(I, sizeof(PyLiteModuleObject)));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->ob_flags = 0;
    obj->attrs = pylt_obj_dict_new(I);
    obj->code = code;
	obj->name = name;
    return obj;
}

void pylt_obj_mod_setattr(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value) {
    pylt_obj_dict_setitem(I, (mod)->attrs, castobj(key), value);
}

PyLiteObject* pylt_obj_mod_getattr(PyLiteInterpreter *I, PyLiteModuleObject *self, PyLiteObject *key) {
    return pylt_obj_dict_getitem(I, self->attrs, key);
}

void pylt_obj_module_free(PyLiteInterpreter *I, PyLiteModuleObject *self) {
    pylt_obj_dict_free(I, self->attrs);
    pylt_free_ex(I, self);
}
