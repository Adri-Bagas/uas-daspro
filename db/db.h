#ifndef DB_H
#define DB_H

#include "../sqlite/sqlite3.h"

/**
 * Opens an existing SQLite3 database or creates a new one if it doesn't exist.
 * * @param db_name The filename of the database (e.g., "mydata.db")
 * @param db A pointer to the sqlite3 pointer variable
 * @return int The result code (SQLITE_OK on success)
 */
int openOrCreateDB(const char* db_name, sqlite3** db);

int migrate_up(sqlite3* db);
int migrate_down(sqlite3* db);

#endif