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

} RouteParam;
void route(Router router, HttpRequest *request, BuffSock *bs);
void router_init(Router *router);
int router_add_route(Router *router, Route route);
void router_destroy(Router *router);

int route_match(StringView route_path, StringView request_path);
