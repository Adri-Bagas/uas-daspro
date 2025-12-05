#ifndef GENERATE_H
#define GENERATE_H

#include "../sqlite/sqlite3.h"
#include "user.h"

int generate_report(sqlite3 *db, User *user, int month, int year) ;

#endif