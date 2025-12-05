#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../sqlite/sqlite3.h"
#include "utils.h"

typedef struct {
    int id;
    int user_id;
    int wallet_id;
    int category_id;
    char *type;          // "income", "expense", etc.
    double amount;
    char *description;
    char *date;          // transaction_date
    
    // JOINED FIELDS (From other tables)
    char *wallet_name;
    char *category_name;
} Transaction;

void free_transaction(Transaction *transaction);
void free_transactions(Transaction **transactions);
Transaction **get_all_transactions_by_user_id(sqlite3 *db, int user_id);
Transaction **get_all_transactions_by_user_id_with_limit(sqlite3 *db, int user_id, int limit);
Transaction **get_transactions_by_date_range(sqlite3 *db, int user_id, const char *start_date, const char *end_date);
Transaction **get_transactions_by_month_year(sqlite3 *db, int user_id, int month, int year);
void show_transactions_table(Transaction **transactions);

#endif