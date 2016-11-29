
#ifndef _WIN32
#ifndef CONSOLE_LINUX_FIX_H
#define CONSOLE_LINUX_FIX_H

int mycrt_read(int fd, void *buffer, unsigned int count);
int mycrt_write(int fd, void *buffer, unsigned int count);


#define read(fd,buffer,count)       mycrt_read(fd,buffer,count)
#define write(fd,buffer,count)      mycrt_write(fd,buffer,count)
//#define isatty(fd)                  mycrt_isatty(fd)

#endif
#endif
