
#ifndef WIN32_INTEROP_FDAPI_FIX_M_H
#define WIN32_INTEROP_FDAPI_FIX_M_H

int mycrt_read(int fd, void *buffer, unsigned int count);
int mycrt_write(int fd, void *buffer, unsigned int count);
int mycrt_isatty(int fd);


#define read(fd,buffer,count)       mycrt_read(fd,buffer,count)
#define write(fd,buffer,count)      mycrt_write(fd,buffer,count)
#define isatty(fd)                  mycrt_isatty(fd)

#endif

/*
* Copyright (c), Microsoft Open Technologies, Inc.
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WIN32_INTEROP_APIS_M_H
#define WIN32_INTEROP_APIS_M_H

#include <stdint.h>
#include <Windows.h>
#include <stdio.h>      // for rename

// API replacement for non-fd stdio functions
#define fseeko      _fseeki64
#define ftello      _ftelli64
#define snprintf    _snprintf
#define strcasecmp  _stricmp
#define strtoll     _strtoi64

#ifdef _WIN64
#define strtol      _strtoi64
#define strtoul     _strtoui64
#endif

#define sleep(x) Sleep((x)*1000)
/* Redis calls usleep(1) to give thread some time.
* Sleep(0) should do the same on Windows.
* In other cases, usleep is called with millisec resolution
* which can be directly translated to WinAPI Sleep() */
#undef usleep
#define usleep(x) (x == 1) ? Sleep(0) : Sleep((int)((x)/1000))


/* following defined to choose little endian byte order */
#define __i386__ 1
#if !defined(va_copy)
#define va_copy(d,s)  d = (s)
#endif

#define lseek lseek64

#endif


#ifndef WIN32_INTEROPA_PORTABILITY_H
#define WIN32_INTEROPA_PORTABILITY_H


#ifdef __cplusplus
extern "C"
{
#endif

    /*  Sometimes in the Windows port we make changes from:
    antirez_redis_statement();
    to:
    #ifdef _WIN32
    windows_redis_statement();
    #else
    antirez_redis_statement();
    #endif

    If subsequently antirez changed that code, we might not detect the change during the next merge.
    The INDUCE_MERGE_CONFLICT macro expands to nothing, but it is used to make sure that the original line
    is modified with respect to the antirez version, so that any subsequent modifications will trigger a conflict
    during the next merge.

    Sample usage:
    #ifdef _WIN32
    windows_redis_statement();
    #else
    antirez_redis_statement();          INDUCE_MERGE_CONFLICT
    #endif

    Don't use any parenthesis or semi-colon after INDUCE_MERGE_CONFLICT.
    Use it at the end of a line to preserve the original indentation.
    */
#define INDUCE_MERGE_CONFLICT

    /*  Use WIN_PORT_FIX at the end of a line to mark places where we make changes to the code
    without using #ifdefs. Useful to keep the code more legible. Mainly intended for replacing
    the use of long (which is 64-bit on 64-bit Unix and 32-bit on 64-bit Windows) to portable types.
    In order to be eligible for an inline fix (without #ifdef), the change should be portable back to the Posix version.
    */
#define WIN_PORT_FIX

#ifdef _WIN32
#define IF_WIN32(x, y) x
#define WIN32_ONLY(x) x
#define POSIX_ONLY(x)
#define inline __inline
#else
#define IF_WIN32(x, y) y
#define WIN32_ONLY(x)
#define POSIX_ONLY(x) x
#endif

#ifdef __cplusplus
}
#endif

#endif


/*
* Modified by Henry Rawas (henryr@schakra.com)
*  - make it compatible with Visual Studio builds
*  - added wstrtod to handle INF, NAN
*  - added support for using IOCP with sockets
*/

#ifndef WIN32FIXES_M_H
#define WIN32FIXES_M_H

#pragma warning(error: 4005)
#pragma warning(error: 4013)

#ifdef WIN32
#ifndef _WIN32
#define _WIN32
#endif
#endif

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define __USE_W32_SOCKETS

#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>      // for _O_BINARY
#include <limits.h>     // for INT_MAX

#define WNOHANG 1

/* file mapping */
#define PROT_READ 1
#define PROT_WRITE 2

#define MAP_FAILED   (void *) -1

#define MAP_SHARED 1
#define MAP_PRIVATE 2

#if _MSC_VER < 1800
#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif

#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#endif
#endif

// access check for executable uses X_OK. For Windows use READ access.
#ifndef X_OK
#define X_OK 4
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#endif /* WIN32FIXES_H */
