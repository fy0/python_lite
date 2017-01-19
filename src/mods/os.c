
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "os.h"
#include "../api.h"
#include "../intp.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"


PyLiteModuleObject* pylt_mods_os_loader(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, _S(os));
    
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_ACCMODE", true),  castobj(pylt_obj_int_new(I, 0000003)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_RDONLY", true),   castobj(pylt_obj_int_new(I, 0000000)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_WRONLY", true),   castobj(pylt_obj_int_new(I, 0000001)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_RDWR", true),     castobj(pylt_obj_int_new(I, 0000002)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_CREAT", true),    castobj(pylt_obj_int_new(I, 0000100)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_EXCL", true),     castobj(pylt_obj_int_new(I, 0000200)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_NOCTTY", true),   castobj(pylt_obj_int_new(I, 0000400)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_TRUNC", true),    castobj(pylt_obj_int_new(I, 0001000)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_APPEND", true),   castobj(pylt_obj_int_new(I, 0002000)));
    pylt_obj_mod_setattr(I, mod, pl_strnew_u8(I, "O_NONBLOCK", true), castobj(pylt_obj_int_new(I, 0004000)));

    return mod;
}

pl_bool_t pylt_mods_os_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(os), &pylt_mods_os_loader, true);
}
