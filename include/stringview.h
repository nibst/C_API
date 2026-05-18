#pragma once
#include <stdlib.h>
typedef struct{
    const char* data;
    size_t len;
} StringView;

StringView string_init(const char *cstr);
