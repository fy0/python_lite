
#include "unusualm.h"
#include "../bind.h"
#include "../types/all.h"


PyLiteModuleObject* pylt_mods_unusual_register(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(unusual));
	pylt_obj_mod_setattr(I, mod, _S(unknown), castobj(&PyLiteUnknown));
	pylt_obj_mod_setattr(I, mod, _S(useless), castobj(&PyLiteUseless));
	pylt_obj_mod_setattr(I, mod, _S(param_args), castobj(&PyLiteParamArgs));
	pylt_obj_mod_setattr(I, mod, _S(param_kwargs), castobj(&PyLiteParamKwargs));
	pylt_obj_mod_setattr(I, mod, _S(param_undefined), castobj(&PyLiteParamUndefined));
    return mod;
}
