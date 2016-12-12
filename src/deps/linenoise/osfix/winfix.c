
#ifdef _WIN32

#include <io.h>
#include <Windows.h>
#include "winfix.h"
#include "Win32_ANSI.h"

int mycrt_read(int fd, void *buffer, unsigned int count) {
    //return _read(fd, buffer, count);
    wchar_t *s = (wchar_t *)buffer;
    for (unsigned int i = 0; i < count; ++i) {
        *s++ = fgetwc(stdin);
    }
    return count;
}

int mycrt_write(int fd, void *buffer, unsigned int count) {
    if (fd == _fileno(stdout)) {
        DWORD bytesWritten = 0;
        if (FALSE != ParseAndPrintANSIString(GetStdHandle(STD_OUTPUT_HANDLE), buffer, (DWORD)count, &bytesWritten)) {
            return (int)bytesWritten;
        } else {
            errno = GetLastError();
            return 0;
        }
    } else if (fd == _fileno(stderr)) {
        DWORD bytesWritten = 0;
        if (FALSE != ParseAndPrintANSIString(GetStdHandle(STD_ERROR_HANDLE), buffer, (DWORD)count, &bytesWritten)) {
            return (int)bytesWritten;
        } else {
            errno = GetLastError();
            return 0;
        }
    } else {
        int retval = _write(fd, buffer, (unsigned int)count);
        if (retval == -1) {
            errno = GetLastError();
        }
        return retval;
    }
}

int mycrt_isatty(int fd) {
    return _isatty(fd);
}

#endif
