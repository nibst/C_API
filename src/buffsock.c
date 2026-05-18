#include "buffsock.h"
#include <errno.h>
#include <sys/socket.h>
int buffsock_init(BuffSock *bs, int socket, size_t capacity){
    if (capacity > BUFFER_LIMIT){
        return -1;
    }
    bs->socket = socket;
    bs->buf = malloc(sizeof(char) * capacity);
    if (capacity <= 0){
        capacity = BUFFER_INIT_CAPACITY;
    }
    bs->capacity = capacity;
    bs->len = 0;
    return 0;
}
ssize_t buffsock_read(BuffSock *bs){
    ssize_t bytes_read;
    /* read the request 
     * EINTR = "interrupted by signal".
     * so this while is a way of saying "I didn't actually fail, I was just interrupted, try again".
     * */
    while ((bytes_read = recv(bs->socket, bs->buf + bs->len, bs->capacity - bs->len - 1, 0)) == -1 && errno == EINTR);
    bs->len += bytes_read;
    bs->buf[bs->len] = '\0';
    return bytes_read;
}
void buffsock_free(BuffSock *bs){
    bs->socket = -1;
    free(bs->buf);
    bs->capacity = 0;
    bs->len = 0;
}

void buffsock_clear(BuffSock *bs){
    bs->capacity = BUFFER_INIT_CAPACITY;
    bs->len = 0;
}
