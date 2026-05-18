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

enum Method {
    GET, POST, PUT, DELETE
};

enum Method method_from_str(StringView method);
char* http_response(int status, const char* message);
char* http_ok();
char* http_not_found();
