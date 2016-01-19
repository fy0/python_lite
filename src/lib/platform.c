
#include "platform.h"
#include <stdio.h>
#include <locale.h>
#include <stdarg.h>


#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

void platform_init() {
#ifdef PLATFORM_WINDOWS
    setlocale(LC_CTYPE, "");
#endif
}
