
#ifndef PYLITE_WATCH_H
#define PYLITE_WATCH_H

#include "../types/object.h"

//PyLiteWatchObject;

void pl_obj_watch(PyLiteInterpreter *I, PyLiteObject *container, PyLiteObject *key, PyLiteObject *func);
void pl_obj_unwatch(PyLiteInterpreter *I, PyLiteObject *watch);
void pl_obj_unwatch_all(PyLiteInterpreter *I, PyLiteObject *container);

#endif
