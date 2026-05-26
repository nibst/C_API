
#include "buffsock.h"
#include "http.h"
void handle_get_joke(HttpRequest *request, BuffSock *bs);
void handle_delete_joke(HttpRequest *request, BuffSock *bs);
void handle_insert_joke(HttpRequest *request, BuffSock *bs);
void handle_update_joke(HttpRequest *request, BuffSock *bs);
void handle_joke_count(HttpRequest *request, BuffSock *bs);
void handle_get_all_jokes(HttpRequest *request, BuffSock *bs);
