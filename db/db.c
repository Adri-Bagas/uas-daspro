#include <stdio.h>
#include "migrations.h"
#include "db.h"

int openOrCreateDB(const char* db_name, sqlite3** db) {
    int rc = sqlite3_open(db_name, db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        
        sqlite3_close(*db);
        
        return rc;
    }

    printf("Successfully connected to database: %s\n", db_name);
    return SQLITE_OK;
}