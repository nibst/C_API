#pragma once
#include "picohttpparser.h"
#include "stringview.h"
#include "yyjson.h"

typedef enum{
    PARAM_INT,
    PARAM_STRING,
    PARAM_UNKNOWN
}ParamType;
typedef struct {
    char name[32];
    ParamType type;
    union {
        int i;
        char s[128];
    } value;
} PathParam;
typedef struct {
    StringView method;
    StringView path;
    int minor_version;
    struct phr_header headers[100]; //at some point we should change this to a pointer with arbitrarily size
    size_t num_headers;
    StringView body;
    PathParam params[8];
    size_t num_params;
}HttpRequest;

typedef struct {
    int code;
    const char *name;
} HttpStatus;

enum Method {
   GET, POST, PUT, DELETE
};

enum Method method_from_str(StringView method);
/*
 * If first parameter is NULL, we use default template, which have Connection as keep-alive
 * Message and data could be NULL if you dont wanna send them
 */
char* http_response(const char* response_template, int status, const char* message, yyjson_mut_val *data);
char *http_close_response(int status, const char *message,yyjson_mut_val *data);
char* http_ok();
char* http_not_found();
const char *http_status_name(int code);
PathParam *get_param(char* name, PathParam* params, size_t num_params);

