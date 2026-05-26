CC = gcc
CFLAGS = -Wall -Ilib/picohttpparser -Ilib/yyjson/ -Ilib/bcrypt -Iinclude
LIBS = lib/bcrypt/bcrypt.a -lsqlite3 -lm
SRC = $(wildcard src/*.c) lib/picohttpparser/picohttpparser.c lib/yyjson/yyjson.c

myserver: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o ./bin/myserver
