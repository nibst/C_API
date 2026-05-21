
#include <sys/socket.h>
#include "yyjson.h"
#include "bcrypt.h"
#include "buffsock.h"
#include "http.h"
#include "db.h"
#define SALT_WORKFACTOR 12
void handle_register(HttpRequest *request, BuffSock *bs){
    printf("registering");
    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *name = yyjson_obj_get(root, "username");
    yyjson_val *password = yyjson_obj_get(root, "password");

    // Free the doc
    yyjson_doc_free(doc);
    char hash[BCRYPT_HASHSIZE];
    char salt[BCRYPT_HASHSIZE];
    int ret = bcrypt_gensalt(SALT_WORKFACTOR, salt);
    ret = bcrypt_hashpw((char*)password, salt, hash);
    char * response = http_ok();
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
void handle_login(HttpRequest *request, BuffSock *bs){
    printf("logging in....\n");
    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *name = yyjson_obj_get(root, "username");
    yyjson_val *password = yyjson_obj_get(root, "password");

    // Free the doc
    yyjson_doc_free(doc);
    char hash[BCRYPT_HASHSIZE];
    char salt[BCRYPT_HASHSIZE];

    int ret = bcrypt_gensalt(SALT_WORKFACTOR, salt);
    ret = bcrypt_hashpw((char*)password, salt, hash);
    char * response = http_ok();
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
