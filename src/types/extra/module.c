
#include "module.h"
#include "../common/dict.h"
#include "../../gc.h"

PyLiteModuleObject* pylt_obj_module_new_ex(PyLiteInterpreter *I, PyLiteObject *owner, PyLiteStrObject *name) {
    PyLiteObject_init(I, obj, PyLiteModuleObject, PYLT_OBJ_TYPE_MODULE);
    obj->ob_attrs = pylt_obj_dict_new(I);
    obj->ob_attrs->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    obj->ob_owner = owner;
    obj->name = name;
    return obj;
}

PyLiteModuleObject* pylt_obj_module_new(PyLiteInterpreter *I, PyLiteStrObject *name) {
    return pylt_obj_module_new_ex(I, NULL, name); // owner NULL is interpreter
}

void pylt_obj_mod_setattr(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value) {
    pylt_obj_dict_setitem(I, (mod)->ob_attrs, castobj(key), value);
}

PyLiteObject* pylt_obj_mod_getattr(PyLiteInterpreter *I, PyLiteModuleObject *self, PyLiteStrObject *key) {
    return pylt_obj_dict_getitem(I, self->ob_attrs, castobj(key));
}

void pylt_obj_module_rfree(PyLiteInterpreter *I, PyLiteModuleObject *self) {
    //pylt_obj_dict_free(I, self->ob_attrs);
    pylt_free_ex(I, self);
}

void pylt_obj_module_free(PyLiteInterpreter *I, PyLiteModuleObject *self) {
    pylt_gc_remove(I, castobj(self));
    pylt_obj_module_rfree(I, self);
}
