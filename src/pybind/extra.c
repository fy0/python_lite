
#include "extra.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_range_new(PyLiteState *state, int argc, PyLiteObject **args) {
    pl_int_t start, end;
    if (castnone(args[2]) == &PyLiteNone) {
        start = 0;
        end = castint(args[1])->ob_val;
    } else {
        start = castint(args[1])->ob_val;
        end = castint(args[2])->ob_val;
    }
    return castobj(pylt_obj_range_new(state, start, end, castint(args[3])->ob_val));
}
