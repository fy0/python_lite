
#include "io_crt.h"
#include "misc.h"
#include <errno.h>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include "../deps/linenoise/osfix/Win32_ANSI.h"

static int win32read(HANDLE hIn, uint32_t *c) {
    DWORD foo;
    INPUT_RECORD b;
    KEY_EVENT_RECORD e;
    BOOL altgr;

    while (1) {
        if (!ReadConsoleInput(hIn, &b, 1, &foo)) return 0;
        if (!foo) return 0;

        if (b.EventType == KEY_EVENT && b.Event.KeyEvent.bKeyDown) {

            e = b.Event.KeyEvent;
            *c = b.Event.KeyEvent.uChar.UnicodeChar;

            altgr = e.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED);

            if (e.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED) && !altgr) {

                /* Ctrl+Key */
                switch (*c) {
                    case 'D':
                        *c = 4;
                        return 1;
                    case 'C':
                        *c = 3;
                        return 1;
                    case 'H':
                        *c = 8;
                        return 1;
                    case 'T':
                        *c = 20;
                        return 1;
                    case 'B': /* ctrl-b, left_arrow */
                        *c = 2;
                        return 1;
                    case 'F': /* ctrl-f right_arrow*/
                        *c = 6;
                        return 1;
                    case 'P': /* ctrl-p up_arrow*/
                        *c = 16;
                        return 1;
                    case 'N': /* ctrl-n down_arrow*/
                        *c = 14;
                        return 1;
                    case 'U': /* Ctrl+u, delete the whole line. */
                        *c = 21;
                        return 1;
                    case 'K': /* Ctrl+k, delete from current to end of line. */
                        *c = 11;
                        return 1;
                    case 'A': /* Ctrl+a, go to the start of the line */
                        *c = 1;
                        return 1;
                    case 'E': /* ctrl+e, go to the end of the line */
                        *c = 5;
                        return 1;
                }

                /* Other Ctrl+KEYs ignored */
            } else {

                switch (e.wVirtualKeyCode) {

                    case VK_ESCAPE: /* ignore - send ctrl-c, will return -1 */
                        *c = 3;
                        return 1;
                    case VK_RETURN:  /* enter */
                        *c = 13;
                        return 1;
                    case VK_LEFT:   /* left */
                        *c = 2;
                        return 1;
                    case VK_RIGHT: /* right */
                        *c = 6;
                        return 1;
                    case VK_UP:   /* up */
                        *c = 16;
                        return 1;
                    case VK_DOWN:  /* down */
                        *c = 14;
                        return 1;
                    case VK_HOME:
                        *c = 1;
                        return 1;
                    case VK_END:
                        *c = 5;
                        return 1;
                    case VK_BACK:
                        *c = 8;
                        return 1;
                    case VK_DELETE:
                        *c = 127;
                        return 1;
                    default:
                        if (*c) return 1;
                }
            }
        }
    }

    return -1; /* Makes compiler happy */
}
#endif

int crt_read(PyLiteInterpreter *I, int fd, uint32_t *buffer, pl_uint_t count, pl_bool_t *accepted) {
    if (accepted) *accepted = false;
#ifdef PLATFORM_WINDOWS
    HANDLE hIn;
    if (fd == _fileno(stdin)) hIn = GetStdHandle(STD_INPUT_HANDLE);
    else return -3;

    if (hIn == INVALID_HANDLE_VALUE) {
        errno = ENOTTY;
        return -1;
    }

    if (accepted) *accepted = true;
    for (pl_uint_t i = 0; i < count; ++i) {
        if (win32read(hIn, buffer + i) < 0) return -1;
    }
    return count;
#else
    if (fd != fileno(stdin)) return -3;
    if (accepted) *accepted = true;
    uint32_t *s = (uint32_t *)buffer;
    for (unsigned int i = 0; i < count; ++i) {
        *s++ = fgetwc(stdin);
    }
    return count;
#endif
}

int crt_write(PyLiteInterpreter *I, int fd, uint32_t *buffer, pl_uint_t count, uint32_t ignore, pl_bool_t *accepted) {
    if (accepted) *accepted = false;
#ifdef PLATFORM_WINDOWS
    HANDLE hDev = INVALID_HANDLE_VALUE;

    if (fd == _fileno(stdout)) hDev = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (fd == _fileno(stderr)) hDev = GetStdHandle(STD_ERROR_HANDLE);
    else return -3;

    if (accepted) *accepted = true;
    if (hDev == INVALID_HANDLE_VALUE) {
        errno = GetLastError();
        return -1;
    }

    DWORD bytesWritten = 0;
    wchar_t *wbuf = pylt_malloc(I, count * sizeof(wchar_t));
    if (!ucs4str_to_wchar(buffer, count, wbuf, ignore)) {
        pylt_free(I, wbuf, count * sizeof(wchar_t));
        return -2;
    }

    if (FALSE != ParseAndPrintWString(hDev, wbuf, (DWORD)count, &bytesWritten)) {
        return (int)bytesWritten * sizeof(wchar_t);
    } else {
        errno = GetLastError();
        return -1;
    }
#else
    FILE *fp = NULL;

    if (fd == fileno(stdout)) fp = stdout;
    else if (fd == fileno(stderr)) fp = stderr;
    else return -3;

    // TODO: ignore
    if (accepted) *accepted = true;
    for (unsigned int i = 0; i < count; ++i) {
        fputwc((wchar_t)buffer[i], fp);
    }
    fflush(fp);
    return count;
#endif
}
