
#ifndef PYLITE_TYPES_OBJECT_H
#define PYLITE_TYPES_OBJECT_H

#include "../utils.h"

typedef struct PyLiteMap {
    int map;
} PyLiteMap;

typedef struct PyLiteObject {
    uint32_t ob_type;
    struct PyLiteMap *ob_attrs;
} PyLiteObject;

#define PyLiteObject_HEAD \
    uint32_t ob_type; \
    struct PyLiteDict *ob_attrs;

#endif
