
#ifndef CONSOLE_OS_FIX_H
#define CONSOLE_OS_FIX_H

#include <stdint.h>

int mycrt_read(int fd, void *buffer, unsigned int count);
int mycrt_write(int fd, void *buffer, unsigned int count);

#ifdef _WIN32
int mycrt_isatty(int fd);
int mk_wcswidth(const wchar_t *pwcs, size_t n);
#endif

#endif
