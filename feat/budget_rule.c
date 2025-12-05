#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../db/db.h"
#include "budget_rule.h"

BudgetRule* get_budget_rule_by_user_id(sqlite3 *db, int userId) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, user_id, needs_percentage, wants_percentage, savings_percentage, target_income, created_at FROM BudgetRules WHERE user_id = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_bind_int(stmt, 1, userId);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        BudgetRule *rule = malloc(sizeof(BudgetRule));
        rule->id = sqlite3_column_int(stmt, 0);
        rule->user_id = sqlite3_column_int(stmt, 1);
        rule->needs_percentage = sqlite3_column_int(stmt, 2);
        rule->wants_percentage = sqlite3_column_int(stmt, 3);
        rule->savings_percentage = sqlite3_column_int(stmt, 4);
        rule->target_income = sqlite3_column_double(stmt, 5);
        rule->created_at = strdup((char *)sqlite3_column_text(stmt, 6));

        sqlite3_finalize(stmt);

        return rule;
    }

    sqlite3_finalize(stmt);

    return NULL;
}

int create_budget_rules(sqlite3 *db, int userId, int needs, int wants, int savings, double target_income)
{
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO BudgetRules (user_id, needs_percentage, wants_percentage, savings_percentage, target_income) VALUES (?, ?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, needs);
    sqlite3_bind_int(stmt, 3, wants);
    sqlite3_bind_int(stmt, 4, savings);
    sqlite3_bind_double(stmt, 5, target_income);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_DONE)
    {
        printf("Budget Rule inserted successfully.\n");
    }
    else
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}