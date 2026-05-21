#pragma once
#include "picohttpparser.h"
#include "stringview.h"

typedef struct {
    StringView method;
    StringView path;
    int minor_version;
    struct phr_header headers[100]; //at some point we should change this to a pointer with arbitrarily size
    size_t num_headers;
    StringView body;
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
 * First if first parameter is NULL, we use default template, which have Connection as keep-alive
 */
char* http_response(const char* response_template, int status, const char* message);
char *http_close_response(int status, const char *message);
char* http_ok();
char* http_not_found();
const char *http_status_name(int code);
