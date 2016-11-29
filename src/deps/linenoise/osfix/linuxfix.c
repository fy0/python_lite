
#ifndef _WIN32
#include "linuxfix.h"
#include <wchar.h>
#include <stdio.h>

int mycrt_read(int fd, void *buffer, unsigned int count) {
    wchar_t *s = (wchar_t *)buffer;
    for (unsigned int i = 0; i < count; ++i) {
        *s++ = fgetwc(stdin);
    }
    return count;
}

int mycrt_write(int fd, void *buffer, unsigned int count) {
    FILE *fp = NULL;
    wchar_t *s = (wchar_t *)buffer;

    if (fd == fileno(stdout)) fp = stdout;
    else if (fd == fileno(stderr)) fp = stderr;
    else return 0;

    for (unsigned int i = 0; i < count; ++i) {
        fputwc(s[i], fp);
    }
    fflush(fp);
    return count;
}

#endif
