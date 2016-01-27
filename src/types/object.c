
#include "object.h"
#include "number.h"

PyLiteTable* pylt_obj_table_new(int size) {
    PyLiteTable *tab;
    if (size < 0) return NULL;

    tab = pylt_realloc(NULL, sizeof(PyLiteTable));
    tab->used = 0;
    tab->size = size;
    tab->data = (size > 0) ? pylt_realloc(NULL, sizeof(PyLiteTableEntry) * size) : NULL;

    return tab;
}

void pylt_obj_table_insert(PyLiteTable *tab, PyLiteObject *key, PyLiteObject *val) {
    pylt_obj_hash(key);
}

uint32_t pylt_obj_hash(PyLiteObject *obj) {
    switch (obj->ob_type) {
    case PYLT_OBJ_TYPE_INT: return pylt_obj_int_hash(obj);
    case PYLT_OBJ_TYPE_BOOL: return pylt_obj_int_hash(obj);
    }
}
