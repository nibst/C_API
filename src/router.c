#include "router.h"
#include "http.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <ctype.h>

#include "auth_controller.h"
#include "joke_controller.h"
#include "stringview.h"
int router_add_route(Router *router, Route route){
    if (router->num_route >= router->capacity){
        size_t new_capacity = router->capacity * 2;
        Route *new_routes = realloc(router->routes, sizeof(Route) * new_capacity);
        if (!new_routes) { 
            perror("realloc error");
            return -1;
        }
        router->routes = new_routes;
        router->capacity = new_capacity;
    }
    Route *new_route = &router->routes[router->num_route++];
    new_route->method  = route.method; 
    new_route->path    = strdup(route.path);
    new_route->handler = route.handler;
    return 0;
}
void router_init(Router *router){
    router->capacity = INIT_CAPACITY;
    router->num_route = 0;
    router->routes = malloc(sizeof(Route) * router->capacity);
    router_add_route(router, (Route){POST, "/login", handle_login});
    router_add_route(router, (Route){GET, "/joke", handle_get_joke});
}

void route(Router router, HttpRequest *request, BuffSock *bs) {
    enum Method method = method_from_str(request->method);

    for (int i = 0; i < router.num_route; i++) {
        printf("request path: %.*s\n",(int) request->path.len, request->path.data);
        printf("route path: %s\n", router.routes[i].path);
        route_match(string_init(router.routes[i].path),request->path);
        if (strlen(router.routes[i].path) != request->path.len){
            continue;
        }
        if (router.routes[i].method == method && strncmp(router.routes[i].path, request->path.data, request->path.len) == 0) {
            router.routes[i].handler(request, bs);
            return;
        }
    }
    // send some default msg if the url path does not exist 
    char * response = http_not_found();
    send(bs->socket, response, strlen(response), 0);
    free(response);
    // 404
}
void router_destroy(Router* router){
    if (!router) {
        return;
    }
    for (size_t i = 0; i < router->num_route; i++) {
        free(router->routes[i].path);
    }
    free(router->routes);
}
int is_integer(const char *s) {
    if (*s == '\0') {
        return 0;
    }
    while (*s) {
        if (!isdigit(*s)) {
            return 0;
        }
        s++;
    }
    return 1;
}
/*
 * param_definition will be something like "<int:id>"
 * request_paramater will be something like "4"
 */
RouteParam parse_route_param(char* param_definition, char *request_paramater){

    RouteParam param = {0};
    char *name = "id";
    memcpy(param.name,name, strnlen(name,64));
    param.type = PARAM_INT;
    param.value.i = atoi(request_paramater);
    return param;

}
int route_match(StringView route_path, StringView request_path){
    char route[256];
    char request[256];
    // TODO make it this memcpy + null pointer safer somehow
    // this chunk of code assumes that route_path and request_path have 255 or less chars. Beware 
    // for now lets just treat request, which is client inputed
    if (request_path.len > 255){
        perror("Request path too long");
        return 1;
    }
    memcpy(route,route_path.data,route_path.len);
    memcpy(request, request_path.data,request_path.len);
    route[route_path.len] = '\0';
    request[request_path.len] = '\0';

    char *route_saveptr, *req_saveptr;
    char* route_tok = strtok_r(route,"/",&route_saveptr);
    char* request_tok = strtok_r(request,"/",&req_saveptr);
    fprintf(stderr,"route tok: %s\n", route_tok);
    fprintf(stderr,"req tok: %s\n", request_tok);
    while(route_tok && request_tok){
        if (route_tok[0] == '<'){
             parse_route_param(route_tok, request_tok);
        }
        route_tok = strtok_r(NULL,"/",&route_saveptr);
        request_tok = strtok_r(NULL,"/",&req_saveptr);
        printf("route tok: %s\n", route_tok);
        printf("req tok: %s\n", request_tok);
    }

    return 0;
}

