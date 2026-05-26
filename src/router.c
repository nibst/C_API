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
    router->routes = malloc(sizeof(Route) * router->capacity);
    router->num_route = 0;
    router_add_route(router, (Route){POST, "/login", handle_login});
    router_add_route(router, (Route){GET, "/joke", handle_get_joke});
    router_add_route(router, (Route){GET, "/joke/<int:id>", handle_get_joke});
    router_add_route(router, (Route){PUT, "/joke/<int:id>", handle_update_joke});
    router_add_route(router, (Route){POST, "/joke", handle_insert_joke});
    router_add_route(router, (Route){DELETE, "/joke/<int:id>", handle_delete_joke});
    router_add_route(router, (Route){GET, "/joke/count", handle_joke_count});
    router_add_route(router, (Route){GET, "/joke/all", handle_get_all_jokes});
}

void route(Router router, HttpRequest *request, BuffSock *bs) {
    enum Method method = method_from_str(request->method);

    for (int i = 0; i < router.num_route; i++) {
        printf("request path: %.*s\n",(int) request->path.len, request->path.data);
        printf("route path: %s\n", router.routes[i].path);
        RouteMatch match;
        if (router.routes[i].method == method && (match = route_match(string_init(router.routes[i].path),request->path)).matched) {
            printf("handling...\n");
            memcpy(request->params, match.params, sizeof(PathParam)*match.num_params); 
            request->num_params = match.num_params;
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
//return a true value if is integer, falsy value if not
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
PathParam parse_route_param(char* param_definition, char *request_paramater){
    PathParam param = {0};
    char *saveptr, *saveptr2;
    char* tok = strtok_r(param_definition,">",&saveptr);
    tok = strtok_r(tok,"<",&saveptr);
    char* type = strtok_r(tok,":", &saveptr2);
    printf("type: %s\n", type);
    if (strcmp("int", type) == 0){
        param.type = PARAM_INT;
        param.value.i = atoi(request_paramater);
    }
    else if(strcmp("str",type) == 0){
        param.type = PARAM_STRING;
        snprintf(param.value.s, sizeof(param.value.s), "%s", request_paramater);
    }
    else{
        param.type = PARAM_UNKNOWN;
    }
    char *name = strtok_r(NULL,":",&saveptr2);
    printf("name: %s\n",name);
    snprintf(param.name, sizeof(param.name), "%s", name);
    return param;

}
int get_route_params(PathParam* params, StringView route_path, StringView request_path){

}
RouteMatch route_match(StringView route_path, StringView request_path){
    char route[256];
    char request[256];
    // TODO make it this memcpy + null pointer safer somehow
    // this chunk of code assumes that route_path and request_path have 255 or less chars. Beware 
    // for now lets just treat request, which is client inputed
    RouteMatch match = {.matched = 0};
    if (request_path.len > 255){
        perror("Request path too long");
        return match;
    }
    memcpy(route,route_path.data,route_path.len);
    memcpy(request, request_path.data,request_path.len);
    route[route_path.len] = '\0';
    request[request_path.len] = '\0';

    char *route_saveptr, *req_saveptr;
    char* route_tok = strtok_r(route,"/",&route_saveptr);
    char* request_tok = strtok_r(request,"/",&req_saveptr);
    PathParam param;
    while(route_tok && request_tok){
    fprintf(stderr,"route tok: %s\n", route_tok);
    fprintf(stderr,"req tok: %s,\n", request_tok);
        if (route_tok[0] == '<'){
            fprintf(stderr,"here\n");
            param = parse_route_param(route_tok, request_tok);
            //if param type is int but req tok is not integer(string)
            if (param.type == PARAM_INT && !is_integer(request_tok)){
                return match; //not matching
            }
            //if param type is string but req tok is integer
            if (param.type == PARAM_STRING && is_integer(request_tok)) {
                return match;
            }
            match.params[match.num_params++] = param;
        }
        else{
            size_t route_tok_len = strlen(route_tok);
            size_t request_tok_len = strnlen(request_tok,route_tok_len + 1);// +1 just to give room to be bigger than route_tok
            if(route_tok_len != request_tok_len){
                return match;
            }
            if (strncmp(route_tok, request_tok, route_tok_len ) != 0) {
               return match;
            }
        }

        route_tok = strtok_r(NULL,"/",&route_saveptr);
        request_tok = strtok_r(NULL,"/",&req_saveptr);
    }
    // if any of them arent null it means that one path ended but the other one still had something
    // so they are not equal
    if (route_tok != NULL || request_tok != NULL){
        return match;
    }
    match.matched = 1;
    return match;

}
