#ifndef USER_H
#define USER_H
#include "../sqlite/sqlite3.h"

void insert_user(sqlite3 *db, const char *username, const unsigned char *hash);
#endif