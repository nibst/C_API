#include "joke_controller.h"
#include "db.h"
#include "http.h"
#include "yyjson.h"
#include <string.h>
#include <sys/socket.h>
void handle_get_joke(HttpRequest *request, BuffSock *bs){
    char * joke = get_joke(db);
    char * response = http_response(NULL,200, joke);
    send(bs->socket, response, strlen(response), 0);
    free(joke);
    free(response);
}
void handle_delete_joke(HttpRequest *request, BuffSock *bs){
    int id = 0;
    int status = delete_joke(db, id);

    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *name = yyjson_obj_get(root, "id");
    yyjson_val *password = yyjson_obj_get(root, "password");

    // Free the doc
    yyjson_doc_free(doc);
    char *response;
    if (status != 0){
        response = http_response(NULL,200,"OK");
    }
    else{
        response = http_response(NULL, 400, "Error deleting");
    }
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
