
#include "module.h"
#include "../dict.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteModuleObject *obj = castmod(pylt_realloc(NULL, sizeof(PyLiteModuleObject)));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_dict_new(I);
    obj->code = code;
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
    pylt_free(self);
}
