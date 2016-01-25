
#include "number.h"

bool pylt_obj_is_int(PyLiteObject *obj) {
    return(obj->ob_type == 1);
}

uint32_t pylt_obj_int_hash(PyLiteIntObject *obj) {
    return obj->ob_val;
}

uint32_t pylt_obj_int_cmp(PyLiteObject *a, PyLiteObject *b) {
    if (pylt_obj_is_int(a) && pylt_obj_is_int(b)) {
        if (castint(a)->ob_val < castint(b)->ob_val) return -1;
        else if (castint(a)->ob_val > castint(b)->ob_val) return -1;
        else return 0;
    }
    return -2;
}
