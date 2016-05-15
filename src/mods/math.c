
#include "math.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"


PyLiteObject* pylt_mods_math_sqrt(PyLiteState *state, int argc, PyLiteObject **args) {
    if (args[0]->ob_type == PYLT_OBJ_TYPE_INT) {
        return castobj(pylt_obj_float_new(state, sqrt((double)castint(args[0])->ob_val)));
    } else if (args[0]->ob_type == PYLT_OBJ_TYPE_FLOAT) {
        return castobj(pylt_obj_float_new(state, sqrt(castfloat(args[0])->ob_val)));
    }
    return NULL;
}


PyLiteModuleObject* pylt_mods_math_register(PyLiteState *state) {
    PyLiteModuleObject *mod = pylt_obj_module_new(state, NULL);
    pylt_cfunc_register(state, mod, _NS(state, "sqrt"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_sqrt);
    pylt_obj_mod_setattr(state, mod, _NS(state, "pi"), castobj(pylt_obj_float_new(state, 3.141592653589)));
    return mod;
}
