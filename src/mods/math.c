
#include "math.h"
#include "../state.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"

PyLiteObject* pylt_mods_math_acos(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_acosh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_asin(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_asinh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_atan(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_atan2(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_atanh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_ceil(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_copysign(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_cos(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_cosh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_degrees(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_erf(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_erfc(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_exp(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_expm1(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_fabs(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_factorial(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_floor(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_fmod(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_frexp(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_fsum(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_gamma(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_hypot(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_isfinite(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_isinf(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_isnan(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_ldexp(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_lgamma(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_log(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_log10(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_log1p(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_log2(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_modf(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_pow(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_radians(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_sin(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_sinh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_sqrt(PyLiteState *state, int argc, PyLiteObject **args) {
    if (args[0]->ob_type == PYLT_OBJ_TYPE_INT) {
        return castobj(pylt_obj_float_new(state, sqrt((double)castint(args[0])->ob_val)));
    } else if (args[0]->ob_type == PYLT_OBJ_TYPE_FLOAT) {
        return castobj(pylt_obj_float_new(state, sqrt(castfloat(args[0])->ob_val)));
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_tan(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_tanh(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_trunc(PyLiteState *state, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteModuleObject* pylt_mods_math_register(PyLiteState *state) {
    PyLiteModuleObject *mod = pylt_obj_module_new(state, NULL);

    pylt_cfunc_register(state, mod, _NS(state, "acos"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_acos);
    pylt_cfunc_register(state, mod, _NS(state, "acosh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_acosh);
    pylt_cfunc_register(state, mod, _NS(state, "asin"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_asin);
    pylt_cfunc_register(state, mod, _NS(state, "asinh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_asinh);
    pylt_cfunc_register(state, mod, _NS(state, "atan"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_atan);
    pylt_cfunc_register(state, mod, _NS(state, "atan2"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_atan2);
    pylt_cfunc_register(state, mod, _NS(state, "atanh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_atanh);
    pylt_cfunc_register(state, mod, _NS(state, "ceil"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_ceil);
    pylt_cfunc_register(state, mod, _NS(state, "copysign"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_copysign);
    pylt_cfunc_register(state, mod, _NS(state, "cos"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_cos);
    pylt_cfunc_register(state, mod, _NS(state, "cosh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_cosh);
    pylt_cfunc_register(state, mod, _NS(state, "degrees"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_degrees);
    pylt_cfunc_register(state, mod, _NS(state, "erf"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_erf);
    pylt_cfunc_register(state, mod, _NS(state, "erfc"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_erfc);
    pylt_cfunc_register(state, mod, _NS(state, "exp"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_exp);
    pylt_cfunc_register(state, mod, _NS(state, "expm1"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_expm1);
    pylt_cfunc_register(state, mod, _NS(state, "fabs"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_fabs);
    pylt_cfunc_register(state, mod, _NS(state, "factorial"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_factorial);
    pylt_cfunc_register(state, mod, _NS(state, "floor"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_floor);
    pylt_cfunc_register(state, mod, _NS(state, "fmod"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_fmod);
    pylt_cfunc_register(state, mod, _NS(state, "frexp"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_frexp);
    pylt_cfunc_register(state, mod, _NS(state, "fsum"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_fsum);
    pylt_cfunc_register(state, mod, _NS(state, "gamma"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_gamma);
    pylt_cfunc_register(state, mod, _NS(state, "hypot"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_hypot);
    pylt_cfunc_register(state, mod, _NS(state, "isfinite"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_isfinite);
    pylt_cfunc_register(state, mod, _NS(state, "isinf"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_isinf);
    pylt_cfunc_register(state, mod, _NS(state, "isnan"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_isnan);
    pylt_cfunc_register(state, mod, _NS(state, "ldexp"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_ldexp);
    pylt_cfunc_register(state, mod, _NS(state, "lgamma"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_lgamma);
    pylt_cfunc_register(state, mod, _NS(state, "log"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_log);
    pylt_cfunc_register(state, mod, _NS(state, "log10"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_log10);
    pylt_cfunc_register(state, mod, _NS(state, "log1p"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_log1p);
    pylt_cfunc_register(state, mod, _NS(state, "log2"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_log2);
    pylt_cfunc_register(state, mod, _NS(state, "modf"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_modf);
    pylt_cfunc_register(state, mod, _NS(state, "pow"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_pow);
    pylt_cfunc_register(state, mod, _NS(state, "radians"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_radians);
    pylt_cfunc_register(state, mod, _NS(state, "sin"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_sin);
    pylt_cfunc_register(state, mod, _NS(state, "sinh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_sinh);
    pylt_cfunc_register(state, mod, _NS(state, "sqrt"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_sqrt);
    pylt_cfunc_register(state, mod, _NS(state, "tan"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_tan);
    pylt_cfunc_register(state, mod, _NS(state, "tanh"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_tanh);
    pylt_cfunc_register(state, mod, _NS(state, "trunc"), _NST(state, 1, "val"), NULL, NULL, &pylt_mods_math_trunc);

    pylt_obj_mod_setattr(state, mod, _NS(state, "e"), castobj(pylt_obj_float_new(state, 2.718281828459045)));
    pylt_obj_mod_setattr(state, mod, _NS(state, "pi"), castobj(pylt_obj_float_new(state, 3.141592653589)));
    return mod;
}
