#include <sqlite3.h>
extern sqlite3* db;
sqlite3* init_db();
sqlite3* init_jokes_db();
char *get_joke(sqlite3 *db, int id);
int delete_joke(sqlite3 *db, int id);
int new_joke(sqlite3 *db, const char *data);
int update_joke(sqlite3* db, const char* data, int id);
int joke_count(sqlite3 *db);
void print_all_jokes(sqlite3* db);
char **get_all_jokes(sqlite3 *db);
