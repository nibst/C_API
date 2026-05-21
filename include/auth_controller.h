#pragma once
#include "http.h"
#include "buffsock.h"
void handle_register(HttpRequest *request, BuffSock *bs);
void handle_login(HttpRequest *request, BuffSock *bs);

