#include <netinet/in.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include "picohttpparser.h"
#include "buffsock.h"
#include "stringview.h"
#include "http.h"
#include "router.h"

#define PORT "8080"
#define BACKLOG 10
#define BUF_SIZE 4096
#define REQUEST_LIMIT 1<<20 // 1mb
enum {
    IOError = -1,
    ParseError = -2,
    RequestIsTooLongError = -3
};

int read_body(BuffSock *bs, size_t content_length, size_t request_length){
    int body_received = bs->len - request_length;
    ssize_t rret;
    if (content_length + request_length > REQUEST_LIMIT ){
        return RequestIsTooLongError;
    }
    printf("content-length %zu\n", content_length );
    while (body_received < content_length){
        printf("body received: %d\n", body_received);
        // if its buffer is more than half completed
        if (body_received > bs->capacity/2){
            bs->capacity*= 2;
            printf("realloc: buf=%p, new capacity=%zu\n", bs->buf, bs->capacity);
            char *new_buf = realloc(bs->buf, bs->capacity);
            if (!new_buf) { 
                perror("realloc error");
                break; 
            }
            bs->buf = new_buf;

        }
        rret = buffsock_read(bs);
        body_received += rret;
    }
    return body_received;
}
void log_request(int pret, HttpRequest request){
        printf("request is %d bytes long\n", pret);
        printf("method is %.*s\n", (int)request.method.len, request.method.data);
        printf("path is %.*s\n", (int)request.path.len, request.path.data);
        printf("HTTP version is 1.%d\n", request.minor_version);
        printf("headers:\n");
        for (int i = 0; i != request.num_headers; ++i) {
            printf("%.*s: %.*s\n", (int)request.headers[i].name_len, request.headers[i].name,
                    (int)request.headers[i].value_len, request.headers[i].value);
        }
}
int get_content_length(struct phr_header *headers, size_t num_headers){
    int content_len = 0;
    for (int i = 0; i != num_headers; ++i) {
        //headers.name is not null terminated thats why we need len
        if (strncmp(headers[i].name, "Content-Length", headers[i].name_len) == 0){
            content_len = atoi(headers[i].value);
        }
    }
    return content_len;
}
int log_peer_info(int socket){
    int status = 0;
    // Now we can receive from this connected socket
    struct sockaddr peer_addr;
    socklen_t peer_addr_len = sizeof peer_addr;
    char host[256];
    char serv[128];
    status = getpeername(socket, &peer_addr, &peer_addr_len);
    if (status != 0) {
        perror("Could not get peer name\n");
    }
    status = getnameinfo(&peer_addr, peer_addr_len, host, sizeof(host), serv, sizeof(serv), 0);
    if (status != 0 ){
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
    }
    return status;
}
int parse_request(BuffSock *buffsock, HttpRequest* request){
    int pret = 0;
    size_t prevbuflen = 0;
    request->num_headers = sizeof(request->headers) / sizeof(request->headers[0]);
    ssize_t rret;

    while (1) {
        prevbuflen = buffsock->len;
        //read socket into buffer
        rret = buffsock_read(buffsock);
        if (rret <= 0){
            buffsock_free(buffsock);
            return IOError;
        }
        /* parse the request */
        pret = phr_parse_request(buffsock->buf, buffsock->len, &(request->method.data), &(request->method.len), 
                &(request->path.data), &(request->path.len), &(request->minor_version), request->headers, 
                &(request->num_headers), prevbuflen);
        printf("pret %d\n", pret);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1){
            buffsock_free(buffsock);
            return ParseError;
        }
        /* request is incomplete, continue the loop */
        assert(pret == -2);
        if (buffsock->len == BUF_SIZE){
            perror("TOO LONG");
            buffsock_free(buffsock);
            return RequestIsTooLongError;
        }
    }
    
    log_request(pret, *request);
    return pret;

}
void* handle_connection(void *arg){
    int connected_sock = *(int *)arg;
    free(arg); // free heap allocation yay
    BuffSock buffsock;
    buffsock_init(&buffsock, connected_sock, BUF_SIZE);
    HttpRequest request;
    while(1){
        int bytes = parse_request(&buffsock,&request);
        int content_len = get_content_length(request.headers, request.num_headers);
        size_t buf_size = read_body(&buffsock,content_len, bytes);
        StringView body = {
            .data = buffsock.buf + bytes,
            .len = buf_size
        }; 
        request.body = body;  
        route(&request, &buffsock);
        printf("body: %.*s\n", (int)request.body.len,(request.body.data));
        char *response = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 13\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello World!\n";

        send(connected_sock, response, strlen(response), 0);
        buffsock_clear(&buffsock);
    }
    buffsock_free(&buffsock);
    return NULL;
}
int main(){
    // Accept connection from anywhere
    // on port 8080
    struct addrinfo hints; 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;
    int status;
    struct addrinfo *servinfo;
    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    int sockfd;
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0){
        perror("socket error");
    }
    int yes=1;
    //char yes='1'; // Solaris people use this

    // lose the pesky "Address already in use" error message
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    // bind socket to my port
    if ((status = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) != 0){
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    listen(sockfd,BACKLOG);
    struct sockaddr_storage client_addr;

    socklen_t addr_size = sizeof client_addr;
    while(1){

        int *connected_sock = malloc(sizeof (int));
        if((*connected_sock = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size)) < 0){
            perror("accept error");
        }
        pthread_t thread_id;
        printf("Creating thread\n");
        if (pthread_create(&thread_id, NULL, (void *)handle_connection, (void *)connected_sock) != 0){
            perror("Could not create pthread\n");
            close(*connected_sock);
            continue;
        }
        printf("thread created: %lu\n", thread_id);
        pthread_detach(thread_id);
    }

    perror("hi\n");
    // will never get to here but ok
    close(sockfd);

}
