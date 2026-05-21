#include <sqlite3.h>
extern sqlite3* db;
sqlite3* init_db();
sqlite3* init_jokes_db();
char *get_joke(sqlite3 *db);
int delete_joke(sqlite3 *db, int id);
int new_joke(sqlite3 *db, char *data);
int joke_count(sqlite3 *db);
