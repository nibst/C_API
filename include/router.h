#pragma once
#include "buffsock.h"
#include "http.h"
#define INIT_CAPACITY 8
typedef void (*Handler)(HttpRequest*, BuffSock*);

typedef struct {
    enum Method method;
    char *path;
    Handler handler;
} Route;
typedef struct {
    Route *routes;
    size_t num_route;
    size_t capacity;
} Router;
typedef struct {
    int matched;
    PathParam params[8];
    size_t num_params;
} RouteMatch;
void route(Router router, HttpRequest *request, BuffSock *bs);
void router_init(Router *router);
int router_add_route(Router *router, Route route);
void router_destroy(Router *router);
RouteMatch route_match(StringView route_path, StringView request_path);
int get_route_params(PathParam* params, StringView route_path, StringView request_path);
