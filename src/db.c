
#include "db.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
sqlite3 *db = NULL;

sqlite3* init_db(){
    sqlite3 *db;
    int rc = sqlite3_open("doris.db",&db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    return db;
}

sqlite3 *init_jokes_db() {
    sqlite3 *db;
    int rc = sqlite3_open("jokes.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    return db;
}

char *get_joke(sqlite3 *db) {
    sqlite3_stmt *stmt;
    char *sql ="select joke from Jokes order by random() limit 1;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        char *data = (char *)sqlite3_column_text(stmt, 0);
        char *joke = strdup(data);
        sqlite3_finalize(stmt);
        return joke;
    }
    return NULL;
}

int delete_joke(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char *sql = "delete from Jokes where id=?;"; 
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    if ((rc = sqlite3_bind_int(stmt ,1 ,id)) != SQLITE_OK){
        fprintf(stderr, "Could not bind param:  %s\n", sqlite3_errmsg(db));
        return rc;
    }
    if ((rc = sqlite3_step(stmt)) != SQLITE_ROW) {
        fprintf(stderr, "Could not delete: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    return 0;
}

int new_joke(sqlite3 *db, char *data) {
    sqlite3_stmt *stmt;
    char *sql = "insert into Jokes values(NULL, ?)";
    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    if ((rc = sqlite3_bind_text(stmt ,1 , data, strlen(data),SQLITE_TRANSIENT)) != SQLITE_OK){
        fprintf(stderr, "Could not bind param:  %s\n", sqlite3_errmsg(db));
        return rc;
    }
    if ((rc = sqlite3_step(stmt)) != SQLITE_ROW) {
        fprintf(stderr, "Could not delete: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    return 0;


}

int joke_count(sqlite3 *db) {
    int count = 0;
    sqlite3_stmt *stmt;
    const char *sql = "select count(*) from Jokes;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not get count from db: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        count = sqlite3_column_int(stmt, 0);
    }
    return count;
}
