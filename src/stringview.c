#include "stringview.h"
#include <string.h>

StringView string_init(const char *cstr){
    return (StringView){
        .data = cstr,
        .len = strlen(cstr),
    };
}
