﻿
#include "type.h"
#include "dict.h"
#include "../../intp.h"
#include "../../api.h"

PyLiteTypeObject* pylt_obj_type_new_with_type(PyLiteInterpreter *I, PyLiteStrObject *name, pl_uint32_t instance_type, pl_uint32_t base_type) {
    PyLiteObject_init(I, obj, PyLiteTypeObject, PYLT_OBJ_TYPE_TYPE);
    obj->name = name;
    obj->ob_attrs = pylt_obj_dict_new(I);
    obj->ob_attrs->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    obj->ob_reftype = instance_type;
    obj->ob_base = base_type;
    obj->ob_owner = NULL;
    obj->allow_inherit = true;
    return obj;
}

PyLiteTypeObject* pylt_obj_type_new(PyLiteInterpreter *I, PyLiteStrObject *name, pl_uint32_t base_type, PyLiteDictObject *dict) {
    PyLiteTypeObject *type = pylt_obj_type_new_with_type(I, name, ++I->class_num, base_type);
    type->ob_attrs = (dict) ? pylt_obj_dict_copy(I, dict) : pylt_obj_dict_new(I);
    type->ob_attrs->ob_flags |= PYLT_OBJ_FLAG_CANFREE;
    return type;
}

PyLiteObject* pylt_obj_type_getattr(PyLiteInterpreter *I, PyLiteTypeObject *self, PyLiteObject *key, pl_bool_t *p_at_type) {
    PyLiteObject *obj;
    if (p_at_type) *p_at_type = false;

    while (true) {
        obj = pylt_obj_dict_getitem(I, self->ob_attrs, key);
        if (obj) return obj;
        if (self->ob_reftype == PYLT_OBJ_TYPE_OBJ) break;
        self = pl_type_by_code(I, self->ob_base);
        //if (p_at_type) *p_at_type = true;
    }

    if (self->ob_reftype != PYLT_OBJ_TYPE_TYPE) {
        obj = pylt_obj_dict_getitem(I, pl_type_by_code(I, PYLT_OBJ_TYPE_TYPE)->ob_attrs, key);
        if (p_at_type) *p_at_type = true;
        if (obj) return obj;
    }

    return NULL;
}


void pylt_obj_type_setattr(PyLiteInterpreter *I, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value) {
    pylt_obj_dict_setitem(I, self->ob_attrs, key, value);
}

pl_bool_t pylt_obj_type_delattr(PyLiteInterpreter *I, PyLiteTypeObject *self, PyLiteObject* key) {
    return pylt_obj_dict_remove(I, self->ob_attrs, key);
}


pl_uint32_t pylt_obj_type_hash(PyLiteInterpreter *I, PyLiteTypeObject *self) {
    return self->ob_reftype;
}

void pylt_obj_type_rfree(PyLiteInterpreter *I, PyLiteTypeObject *self) {
    //pylt_obj_dict_free(I, self->ob_attrs);
    pylt_free_ex(I, self);
}

void pylt_obj_type_free(PyLiteInterpreter *I, PyLiteTypeObject *self) {
    pylt_gc_remove(I, castobj(self));
    pylt_obj_type_rfree(I, self);
}
