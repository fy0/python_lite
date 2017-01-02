
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "sys.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"


PyLiteModuleObject* pylt_mods_sys_loader(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(sys));

    return mod;
}

pl_bool_t pylt_mods_sys_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(sys), &pylt_mods_sys_loader, true);
}
