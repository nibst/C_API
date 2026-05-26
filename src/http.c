#include "yyjson.h"
#include "http.h"
#include <string.h>

const char *default_template=
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %zu\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "%s";
const char* close_template=
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "\r\n"
    "%s";
static const HttpStatus HTTP_STATUS_TABLE[] = {
    // 2xx Success
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},

    // 3xx Redirection
    {301, "Moved Permanently"},
    {302, "Found"},
    {304, "Not Modified"},

    // 4xx Client Errors
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {413, "Payload Too Large"},
    {415, "Unsupported Media Type"},
    {429, "Too Many Requests"},

    // 5xx Server Errors
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
};

const char *http_status_name(int code){
    int http_status_table_len = sizeof(HTTP_STATUS_TABLE)/sizeof(HTTP_STATUS_TABLE[0]);
    for(int i = 0; i < http_status_table_len; i++){
        if (code == HTTP_STATUS_TABLE[i].code){
            return HTTP_STATUS_TABLE[i].name;
        }
    }
    return "Unknown";
}
enum Method method_from_str(StringView method){
    if (strncmp(method.data, "POST",   method.len) == 0)
        return POST;
    if (strncmp(method.data, "GET",    method.len) == 0)
        return GET;
    if (strncmp(method.data, "PUT",    method.len) == 0)
        return PUT;
    if (strncmp(method.data, "DELETE", method.len) == 0)
        return DELETE;
    return GET;
}

char* http_ok(){
    return http_response(default_template, 200, "OK", NULL);
}

char* http_not_found(){
    return http_response(default_template, 404, "Not Found", NULL); 
}
char* http_response(const char* response_template, int status, const char* message, yyjson_mut_val *data){
    if (response_template == NULL){
        response_template = default_template;
    }
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);

    if (!doc) {
        return NULL;
    }

    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    char status_str[16];
    snprintf(status_str, sizeof(status_str), "%d", status);

    yyjson_mut_obj_add_str(doc, root, "status", status_str);
    yyjson_mut_obj_add_str(doc, root, "message", message);
    yyjson_mut_obj_add_val(doc, root, "data", data);

    char *json = yyjson_mut_write(doc, 0, NULL);

    yyjson_mut_doc_free(doc);

    if (!json) {
        return NULL;
    }

    size_t content_len = strlen(json);

    size_t response_len = snprintf( NULL, 0, response_template, status, http_status_name(status), content_len, json);
    char *response = malloc(response_len + 1);

    if (!response) {
        free(json);
        return NULL;
    }

    snprintf(response, response_len + 1, response_template, status, http_status_name(status), content_len, json);

    free(json);

    return response;
}
char *http_close_response(int status, const char *message, yyjson_mut_val *data){
    return http_response(close_template, status, message, data);
}
PathParam *get_param(char* name, PathParam* params, size_t num_params){
    for (int i = 0; i < num_params; i++){
        if (strcmp(name, params[i].name) == 0){
            return &params[i];
        }
    }
    return NULL;
}
