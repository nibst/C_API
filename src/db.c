
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

char *get_joke(sqlite3 *db, int id) {
    //if id is invalid then get random joke
    sqlite3_stmt *stmt;
    if (id < 0){
        char *sql ="select joke from Jokes order by random() limit 1;";
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
            return NULL;
        }

    }
    else{
        char *sql ="select joke from Jokes where id=?;";
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
            return NULL;
        }
        if ((rc = sqlite3_bind_int(stmt ,1 ,id)) != SQLITE_OK){
            fprintf(stderr, "Could not bind param:  %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return NULL;
        }
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
        sqlite3_finalize(stmt);
        return rc;
    }
    if ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        fprintf(stderr, "Could not delete: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);

        return rc;
    }
    fprintf(stderr,"rc: %d\n", rc);
    return 0;
}

int new_joke(sqlite3 *db, const char *data) {
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
    if ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        fprintf(stderr, "Could not insert: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    return 0;


}

int update_joke(sqlite3* db, const char* data, int id){
    if (id < 0){
        fprintf(stderr, "Invalid Id");
    }
    sqlite3_stmt *stmt;
    char *sql = "update Jokes set joke=? where id=?";
    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not read DB: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    printf("joke : %s\n",data);
    if ((rc = sqlite3_bind_text(stmt ,1 , data, strlen(data),SQLITE_TRANSIENT)) != SQLITE_OK){
        fprintf(stderr, "Could not bind param:  %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    if ((rc = sqlite3_bind_int(stmt , 2 , id)) != SQLITE_OK){
        fprintf(stderr, "Could not bind param:  %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    if ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
        fprintf(stderr, "Could not update: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
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
    sqlite3_finalize(stmt);
    return count;
}

void print_all_jokes(sqlite3* db){
    sqlite3_stmt *stmt;
    const char *sql ="select * from Jokes;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not get count from db: %s\n", sqlite3_errmsg(db));
        return ;
    }
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        int id = (int)sqlite3_column_int(stmt, 0);
        char *joke= (char *)sqlite3_column_text(stmt, 1);
        printf("{id: %d, joke: %s}\n",id,joke);
    }
    sqlite3_finalize(stmt);
    return; 
}
char **get_all_jokes(sqlite3 *db){
    sqlite3_stmt *stmt;
    const char *sql = "select joke from Jokes;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not get jokes: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    size_t capacity = 8; // magic number, idk
    size_t count = 0;
    char **jokes = malloc(capacity * sizeof(char *));

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        char *joke= (char *)sqlite3_column_text(stmt, 0);
        if (count + 1 >= capacity){
            capacity *= 2;
            jokes = realloc(jokes, capacity * sizeof(char*));
        }
        jokes[count++] = strdup(joke);

    }
    jokes[count] = NULL; //just so we know when it ends
    sqlite3_finalize(stmt);
    return jokes;
}
