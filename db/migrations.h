#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include "../sqlite/sqlite3.h"

int migrate_up(sqlite3* db);
int migrate_down(sqlite3* db);

#endif