#ifndef MENU_H
#define MENU_H
#include "utils.h"
#include "user.h"
#include "wallet.h"
#include "budget_rule.h"
#include "income.h"
#include "spending.h"

void menu(sqlite3 *db, User* user);

#endif