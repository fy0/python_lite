
#ifndef PYLITE_IO_CRT_H
#define PYLITE_IO_CRT_H

#include "config.h"

int crt_read(PyLiteInterpreter *I, int fd, uint32_t *buffer, pl_uint_t count, pl_bool_t *accepted);
int crt_write(PyLiteInterpreter *I, int fd, uint32_t *buffer, pl_uint_t count, uint32_t ignore, pl_bool_t *accepted);

#endif
