
// sys 必须在 io 模块之后注册

#ifndef PYLITE_MODS_SYS_H
#define PYLITE_MODS_SYS_H

#include "../types/all.h"

PyLiteModuleObject* pylt_mods_sys_register(PyLiteInterpreter *I);

#endif
