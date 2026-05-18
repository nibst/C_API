#include "buffsock.h"
#include "http.h"
typedef void (*Handler)(HttpRequest*, BuffSock*);

typedef struct {
    enum Method method;
    char *path;
    Handler handler;
} Route;

extern Route routes[];
extern int num_routes;

void route(HttpRequest *request, BuffSock *bs);
