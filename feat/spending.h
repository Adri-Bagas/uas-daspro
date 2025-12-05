#ifndef SPENDING_H
#define SPENDING_H

#include "../sqlite/sqlite3.h"

// int insert_income(sqlite3 *db, double amount, int userId, int wallet_id, char *wallet_name);
int create_spending(sqlite3 *db, int userId, int wallet_id, char *wallet_name, int category_id);
#endif