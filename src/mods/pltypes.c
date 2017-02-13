
#include "unusualm.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"
#include "../pybind/typebind.h"


PyLiteModuleObject* pylt_mods_pltypes_loader(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL);
    pylt_bind_all_types_register(I, mod);
    return mod;
}

pl_bool_t pylt_mods_pltypes_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(pltypes), &pylt_mods_pltypes_loader, true);
}
