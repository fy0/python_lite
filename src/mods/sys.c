
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "sys.h"
#include "../api.h"
#include "../intp.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"


PyLiteModuleObject* pylt_mods_sys_loader(PyLiteInterpreter *I) {
	PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, _S(sys));

    PyLiteModuleObject *mio = pl_getmod(I, _S(io));
    PyLiteTypeObject *tTextIO = pl_gettype(I, mio, _S(TextIO));

    PyLiteObject *obj_stdin = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    pylt_obj_setattr(I, obj_stdin, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, I->sys.cin, false)));
    pylt_obj_mod_setattr(I, mod, _S(stdin_), obj_stdin);

    PyLiteObject *obj_stdout = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    pylt_obj_setattr(I, obj_stdout, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, I->sys.cout, false)));
    pylt_obj_mod_setattr(I, mod, _S(stdout_), obj_stdout);

    PyLiteObject *obj_stderr = pylt_obj_cutstom_create(I, tTextIO->ob_reftype, NULL);
    pylt_obj_setattr(I, obj_stderr, castobj(_S(__cobj__)), castobj(pylt_obj_cptr_new(I, stderr, false)));
    pylt_obj_mod_setattr(I, mod, _S(stderr_), obj_stderr);

    return mod;
}

pl_bool_t pylt_mods_sys_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(sys), &pylt_mods_sys_loader, true);
}
