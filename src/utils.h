
#ifndef PYLITE_UTILS_H
#define PYLITE_UTILS_H

#include "lib/platform.h"
#include "lib/utf8_lite.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

void* pylt_realloc(void* m, size_t size);
void pylt_free(void* m);

typedef struct pylt_Stack {
    void* data;
    int top;
    int len;
} pylt_Stack;


#define stack_init(_s, _type, _len) { (_s).data = _len ? pylt_realloc(NULL, sizeof(_type) * _len) : NULL; (_s).top = -1; (_s).len = _len; }
#define stack_get_top(_s, _type) ((_type*)(_s.data) + _s.top)
#define stack_empty(_s) (_s.top == -1)
#define stack_push(_s, _type) ((_type*)(_s.data) + ++(_s.top))
#define stack_pop(_s, _type) ((_type*)((_s).data) + ((_s).top)--)
#define stack_check(_s, _type, _step) if (_s.top == _s.len || _s.len == 0) { _s.len += _step; _s.data = _s.data ? pylt_realloc(_s.data, sizeof(_type) * _s.len) : _new(_type, _s.len);}
#define stack_free(_s) pylt_free((_s).data);
#define stack_copy(_s, _dest, _type) { (_dest).data = pylt_realloc(NULL, sizeof(_type) * _len); memcpy((_dest).data, (_s).data, sizeof(_type) * (_s).len); }

#endif

