
#include <math.h>
#include "math.h"
#include "../intp.h"
#include "../debug.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"

#define MATH_PI       3.14159265358979323846
#define get_val(i) ((i)->ob_type == PYLT_OBJ_TYPE_FLOAT) ? (castfloat((i))->ob_val) : (castint((i))->ob_val)


PyLiteObject* pylt_mods_math_acos(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, acos(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_acosh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, acosh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_asin(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, asin(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_asinh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, asinh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}



PyLiteObject* pylt_mods_math_atan(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, atan(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_atan2(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, atan2(get_val(args[0]), get_val(args[1]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_atanh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, atanh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_ceil(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, ceil(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_copysign(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, copysign(get_val(args[0]), get_val(args[1]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_cos(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, cos(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_cosh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, cosh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}

PyLiteObject* pylt_mods_math_degrees(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, get_val(args[0]) * 180.0 / MATH_PI));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_erf(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, erf(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_erfc(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, erfc(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_exp(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, exp(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_expm1(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, expm1(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_fabs(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, fabs(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_factorial(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return NULL;
}


PyLiteObject* pylt_mods_math_floor(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, floor(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_fmod(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, fmod(get_val(args[0]), get_val(args[1]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_frexp(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return NULL;
}


PyLiteObject* pylt_mods_math_fsum(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return NULL;
}


PyLiteObject* pylt_mods_math_gamma(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return NULL;
}

PyLiteObject* pylt_mods_math_hypot(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return NULL;
}


PyLiteObject* pylt_mods_math_isfinite(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, isfinite(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_isinf(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, isinf(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_isnan(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, isnan(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_ldexp(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, ldexp(get_val(args[0]), (int)(get_val(args[1])))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_lgamma(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, lgamma(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_log(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, log(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_log10(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, log10(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_log1p(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, log1p(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_log2(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, log2(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_modf(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    /*if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, modf(get_val(args[0]), get_val(args[1]))));
    } else {
        // TODO
    }*/
    return NULL;
}


PyLiteObject* pylt_mods_math_pow(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0]) && pl_isnum(args[1])) {
        return castobj(pylt_obj_float_new(I, pow(get_val(args[0]), get_val(args[1]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_radians(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, get_val(args[0]) * MATH_PI / 180.0));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_sin(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, sin(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_sinh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, sinh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_sqrt(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, sqrt(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_tan(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, tan(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteObject* pylt_mods_math_tanh(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    if (pl_isnum(args[0])) {
        return castobj(pylt_obj_float_new(I, tanh(get_val(args[0]))));
    } else {
        // TODO
    }
    return NULL;
}


PyLiteModuleObject* pylt_mods_math_register(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL);

    pylt_cfunc_register(I, mod, _NS(I, "acos"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_acos);
    pylt_cfunc_register(I, mod, _NS(I, "acosh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_acosh);
    pylt_cfunc_register(I, mod, _NS(I, "asin"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_asin);
    pylt_cfunc_register(I, mod, _NS(I, "asinh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_asinh);
    pylt_cfunc_register(I, mod, _NS(I, "atan"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_atan);
    pylt_cfunc_register(I, mod, _NS(I, "atan2"), _NST(I, 2, "y", "x"), NULL, NULL, &pylt_mods_math_atan2);
    pylt_cfunc_register(I, mod, _NS(I, "atanh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_atanh);
    pylt_cfunc_register(I, mod, _NS(I, "ceil"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_ceil);
    pylt_cfunc_register(I, mod, _NS(I, "copysign"), _NST(I, 2, "x", "y"), NULL, NULL, &pylt_mods_math_copysign);
    pylt_cfunc_register(I, mod, _NS(I, "cos"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_cos);
    pylt_cfunc_register(I, mod, _NS(I, "cosh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_cosh);
    pylt_cfunc_register(I, mod, _NS(I, "degrees"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_degrees);
    pylt_cfunc_register(I, mod, _NS(I, "erf"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_erf);
    pylt_cfunc_register(I, mod, _NS(I, "erfc"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_erfc);
    pylt_cfunc_register(I, mod, _NS(I, "exp"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_exp);
    pylt_cfunc_register(I, mod, _NS(I, "expm1"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_expm1);
    pylt_cfunc_register(I, mod, _NS(I, "fabs"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_fabs);
    pylt_cfunc_register(I, mod, _NS(I, "factorial"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_factorial);
    pylt_cfunc_register(I, mod, _NS(I, "floor"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_floor);
    pylt_cfunc_register(I, mod, _NS(I, "fmod"), _NST(I, 2, "x", "y"), NULL, NULL, &pylt_mods_math_fmod);
    pylt_cfunc_register(I, mod, _NS(I, "frexp"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_frexp);
    pylt_cfunc_register(I, mod, _NS(I, "fsum"), _NST(I, 1, "iterable"), NULL, NULL, &pylt_mods_math_fsum);
    pylt_cfunc_register(I, mod, _NS(I, "gamma"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_gamma);
    pylt_cfunc_register(I, mod, _NS(I, "hypot"), _NST(I, 2, "x", "y"), NULL, NULL, &pylt_mods_math_hypot);
    pylt_cfunc_register(I, mod, _NS(I, "isfinite"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_isfinite);
    pylt_cfunc_register(I, mod, _NS(I, "isinf"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_isinf);
    pylt_cfunc_register(I, mod, _NS(I, "isnan"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_isnan);
    pylt_cfunc_register(I, mod, _NS(I, "ldexp"), _NST(I, 2, "x", "i"), NULL, NULL, &pylt_mods_math_ldexp);
    pylt_cfunc_register(I, mod, _NS(I, "lgamma"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_lgamma);
    pylt_cfunc_register(I, mod, _NS(I, "log"), _NST(I, 2, "x", "base"), NULL, NULL, &pylt_mods_math_log);
    pylt_cfunc_register(I, mod, _NS(I, "log10"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_log10);
    pylt_cfunc_register(I, mod, _NS(I, "log1p"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_log1p);
    pylt_cfunc_register(I, mod, _NS(I, "log2"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_log2);
    pylt_cfunc_register(I, mod, _NS(I, "modf"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_modf);
    pylt_cfunc_register(I, mod, _NS(I, "pow"), _NST(I, 2, "x", "y"), NULL, NULL, &pylt_mods_math_pow);
    pylt_cfunc_register(I, mod, _NS(I, "radians"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_radians);
    pylt_cfunc_register(I, mod, _NS(I, "sin"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_sin);
    pylt_cfunc_register(I, mod, _NS(I, "sinh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_sinh);
    pylt_cfunc_register(I, mod, _NS(I, "sqrt"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_sqrt);
    pylt_cfunc_register(I, mod, _NS(I, "tan"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_tan);
    pylt_cfunc_register(I, mod, _NS(I, "tanh"), _NST(I, 1, "x"), NULL, NULL, &pylt_mods_math_tanh);
    //pylt_cfunc_register(I, mod, _NS(I, "trunc"), _NST(I, 1, "val"), NULL, NULL, &pylt_mods_math_trunc);

    pylt_obj_mod_setattr(I, mod, _NS(I, "e"), castobj(pylt_obj_float_new(I, 2.718281828459045)));
    pylt_obj_mod_setattr(I, mod, _NS(I, "pi"), castobj(pylt_obj_float_new(I, MATH_PI)));
    return mod;
}
