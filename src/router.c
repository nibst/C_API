#include "router.h"
#include "http.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "yyjson.h"

void handle_login(HttpRequest *request, BuffSock *bs){
    printf("logging in....\n");
    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    // Get root["joke"]
    yyjson_val *name = yyjson_obj_get(root, "joje");
    printf("name: %s\n", yyjson_get_str(name));
    printf("name length:%d\n", (int)yyjson_get_len(name));

    // Get root["star"]
    yyjson_val *star = yyjson_obj_get(root, "star");
    printf("star: %d\n", (int)yyjson_get_int(star));

    // Get root["hits"], iterate over the array
    yyjson_val *hits = yyjson_obj_get(root, "hits");
    size_t idx, max;
    yyjson_val *hit;
    yyjson_arr_foreach(hits, idx, max, hit) {
        printf("hit%d: %d\n", (int)idx, (int)yyjson_get_int(hit));
    }

    // Free the doc
    yyjson_doc_free(doc);

    char * response = http_ok();
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
Route routes[] = {
    { POST, "/login", handle_login },
};

int num_routes = sizeof(routes) / sizeof(routes[0]);

void route(HttpRequest *request, BuffSock *bs) {
    enum Method method = method_from_str(request->method);

    for (int i = 0; i < num_routes; i++) {
        printf("request path: %.*s\n",(int) request->path.len, request->path.data);
        printf("route path: %s\n", routes[i].path);
        if (strlen(routes[i].path) != request->path.len){
            continue;
        }
        if (routes[i].method == method && strncmp(routes[i].path, request->path.data, request->path.len) == 0) {
            routes[i].handler(request, bs);
            return;
        }
    }
    // send some default msg if the url path does not exist 
    char * response = http_not_found();
    send(bs->socket, response, strlen(response), 0);
    free(response);
    // 404
}
