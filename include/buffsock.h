#pragma once
#include <stdlib.h>
#define  BUFFER_LIMIT 1<<20 // 1mb
#define  BUFFER_INIT_CAPACITY 4096
typedef struct{
   int socket;
   char *buf;
   size_t len;
   size_t capacity;
}BuffSock;

// Returns -1 on error (max buffer capacity exceeded) and 0 on success
int buffsock_init(BuffSock *bs, int socket, size_t capacity);
ssize_t buffsock_read(BuffSock *bs);
void buffsock_destroy(BuffSock *bs);
/* Clears the buffer, doesnt clear the socket file descriptor */
void buffsock_clear(BuffSock *bs);
