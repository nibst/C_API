CC = gcc
CFLAGS = -Wall -Ilib/picohttpparser -Ilib/yyjson/ -Iinclude

SRC = $(wildcard src/*.c) lib/picohttpparser/picohttpparser.c lib/yyjson/yyjson.c

myserver: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o ./bin/myserver
