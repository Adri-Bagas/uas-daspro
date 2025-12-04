#ifndef MENU_H
#define MENU_H

#include "../sqlite/sqlite3.h"
#include "user.h"

void menu(sqlite3 *db, User* user);

#endif