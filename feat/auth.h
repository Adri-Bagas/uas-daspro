#ifndef AUTH_H
#define AUTH_H
#include "../db/db.h"
#include "user.h"

User* login(sqlite3 *db);
void logout();

#endif