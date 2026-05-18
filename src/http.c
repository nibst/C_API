#include "yyjson.h"
#include "http.h"
#include <string.h>
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
    return http_response(200, "OK");
}

char* http_not_found(){
    return http_response(404, "Not Found"); 
}

char *http_response(int status, const char *message) {
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

    char *json = yyjson_mut_write(doc, 0, NULL);

    yyjson_mut_doc_free(doc);

    if (!json) {
        return NULL;
    }

    size_t content_len = strlen(json);

    const char *template =
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "%s";

    size_t response_len = snprintf( NULL, 0, template, status, message, content_len, json);
    char *response = malloc(response_len + 1);

    if (!response) {
        free(json);
        return NULL;
    }

    snprintf(response, response_len + 1, template, status, message, content_len, json);

    free(json);

    return response;
}
