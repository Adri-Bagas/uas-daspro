#ifndef BUDGET_RULE_H
#define BUDGET_RULE_H

#include "../db/db.h"
typedef struct
{
    int id;
    int user_id;
    int needs_percentage;
    int wants_percentage;
    int savings_percentage;
    double target_income;
    char *created_at;
} BudgetRule;

BudgetRule* get_budget_rule_by_user_id(sqlite3 *db, int userId);
int create_budget_rules(sqlite3 *db, int userId, int needs, int wants, int savings, double target_income);

#endif