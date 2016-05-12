
#include "extra.h"
#include "../types/all.h"

PyLiteObject* pylt_cls_method_range_new(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_range_new(state, castint(args[1])->ob_val, castint(args[2])->ob_val, castint(args[3])->ob_val));
}
