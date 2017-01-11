
#include "unusualm.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"


PyLiteModuleObject* pylt_mods_unusual_loader(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, NULL);
    pylt_obj_mod_setattr(I, mod, _S(useless), castobj(&PyLiteUseless));
    pylt_obj_mod_setattr(I, mod, _S(unknown), castobj(&PyLiteNotImplemented));
    pylt_obj_mod_setattr(I, mod, _S(param_args), castobj(&PyLiteParamArgs));
    pylt_obj_mod_setattr(I, mod, _S(param_kwargs), castobj(&PyLiteParamKwargs));
    pylt_obj_mod_setattr(I, mod, _S(param_undefined), castobj(&PyLiteParamUndefined));
    return mod;
}

pl_bool_t pylt_mods_unusual_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(unusual), &pylt_mods_unusual_loader, true);
}
