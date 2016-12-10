
#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#if defined(_WIN32) && !defined(_WIN32_WCE)
#define PLATFORM_WINDOWS
#else
#define PLATFORM_LINUX
#endif

#ifdef _MSC_VER
#define _INLINE __inline
#pragma execution_character_set("utf-8")
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable: 4996)
#else
#define _INLINE inline
#endif

#define SIZEOF_LONG_LONG sizeof(long long)
#define MAX_LONG_LONG_CHARS (2 + (SIZEOF_LONG_LONG*53-1) / 22)


void printf_u8(const char *fmt, ...);

void platform_init();

#endif
