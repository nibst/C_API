#include "joke_controller.h"
#include "db.h"
#include "http.h"
#include "yyjson.h"
#include <string.h>
#include <sys/socket.h>
#include <math.h>
void handle_get_joke(HttpRequest *request, BuffSock *bs){
    int id = -1;
    if (request->num_params != 0){
        id = get_param("id", request->params, request->num_params)->value.i;
    }
    char * joke = get_joke(db, id);
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *data = yyjson_mut_str(doc, joke);
    char * response = http_response(NULL,200, NULL, data);
    yyjson_mut_doc_free(doc);
    printf("%s\n",response);
    send(bs->socket, response, strlen(response), 0);
    free(joke);
    free(response);
}
void handle_delete_joke(HttpRequest *request, BuffSock *bs){
    int id = -1;
    if (request->num_params != 0){
        id = get_param("id", request->params, request->num_params)->value.i;
    }
    int status = delete_joke(db, id);

    char *response;
    if (status == 0){
        response = http_response(NULL,200,"OK", NULL);
    }
    else{
        response = http_response(NULL, 400, "Error deleting", NULL);
    }
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
void handle_insert_joke(HttpRequest *request, BuffSock *bs){

    int id = 0;

    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *joke = yyjson_obj_get(root, "joke");
    int status = new_joke(db, yyjson_get_str(joke));
    // Free the doc
    yyjson_doc_free(doc);
    char *response;
    if (status == 0){
        response = http_response(NULL,200,"OK",NULL);
    }
    else{
        response = http_response(NULL, 400, "Error inserting", NULL);
    }
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
void handle_update_joke(HttpRequest *request, BuffSock *bs){

    int id = -1;
    if (request->num_params != 0){
        id = get_param("id", request->params, request->num_params)->value.i;
    }
    // Read JSON and get root
    yyjson_doc *doc = yyjson_read(request->body.data, request->body.len, 0);
    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *joke = yyjson_obj_get(root, "joke");

    int status = update_joke(db, yyjson_get_str(joke), id);
    // Free the doc
    yyjson_doc_free(doc);
    char *response;
    if (status == 0){
        response = http_response(NULL,200,"OK", NULL);
    }
    else{
        response = http_response(NULL, 400, "Error updating", NULL);
    }
    send(bs->socket, response, strlen(response), 0);
    free(response);
}
void handle_joke_count(HttpRequest *request, BuffSock *bs){

    // Read JSON and get root
    int count = joke_count(db);
    char *response;
    if (count >= 0){
        char *buffer = malloc(sizeof(char)*(int)log10(count)); //this makes sure buffer is large enough
        sprintf(buffer, "%d", count);
        yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
        yyjson_mut_val *data = yyjson_mut_str(doc, buffer);
        response = http_response(NULL,200, NULL, data);
        yyjson_mut_doc_free(doc);
    }
    else{
        response = http_response(NULL, 400, "Error on joke count", NULL);
    }
    print_all_jokes(db);
    send(bs->socket, response, strlen(response), 0);
    free(response);

}
void handle_get_all_jokes(HttpRequest *request, BuffSock *bs){
    
    // Read JSON and get root
    char** jokes = get_all_jokes(db);
    char *response;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    if (!doc) {
        response = http_close_response(500, "Internal error on json lib: error initializing", NULL);
        send(bs->socket, response, strlen(response), 0);
        free(response);
        return;
    }

    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    size_t count = 0;
    for (int i =0; jokes[i] != NULL; i++){
        count++;
    }
    yyjson_mut_val *json_jokes = yyjson_mut_arr_with_str(doc,(const char**)jokes,count);
    yyjson_mut_obj_add_val(doc, root, "jokes", json_jokes);


    response = http_response(NULL, 200, NULL, root);
    yyjson_mut_doc_free(doc);
    send(bs->socket, response, strlen(response), 0);
    free(response);
    print_all_jokes(db);
}
