
#include "module.h"
#include "../dict.h"

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeObject *code) {
    PyLiteModuleObject *obj = castmod(pylt_realloc(NULL, sizeof(PyLiteModuleObject)));
    obj->ob_type = PYLT_OBJ_TYPE_MODULE;
    obj->attrs = pylt_obj_dict_new(state);
    obj->code = code;
    return obj;
}

void pylt_obj_mod_setattr(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value) {
    pylt_obj_dict_setitem(state, (mod)->attrs, castobj(key), value);
}

PyLiteObject* pylt_obj_mod_getattr(PyLiteState *state, PyLiteModuleObject *self, PyLiteObject *key) {
    return pylt_obj_dict_getitem(state, self->attrs, key);
}

void pylt_obj_module_free(PyLiteState *state, PyLiteModuleObject *self) {
    pylt_obj_dict_free(state, self->attrs);
    pylt_free(self);
}
