#include <stdio.h>
#include <string.h>
#include "seeder.h"

typedef struct {
    char *name;
    char *type;
    char *budget_bucket; // Can be NULL
} CategorySeed;

int seed_default_categories(sqlite3 *db) {
    // List of default categories (English)
    CategorySeed defaults[] = {
        
        // NEEDS (50%)
        {"Food & Groceries", "expense", "Needs"},
        {"Transportation", "expense", "Needs"},
        {"Rent / Mortgage", "expense", "Needs"},
        {"Utilities (Electric, Water, Internet)", "expense", "Needs"},
        {"Phone & Data", "expense", "Needs"},
        {"Health", "expense", "Needs"},
        {"Education", "expense", "Needs"},

        // WANTS (30%)
        {"Dining Out", "expense", "Wants"},
        {"Entertainment", "expense", "Wants"},
        {"Online Shopping", "expense", "Wants"},
        {"Hobbies & Gaming", "expense", "Wants"},
        {"Personal Care", "expense", "Wants"},
        {"Subscriptions", "expense", "Wants"},
        {"Travel / Vacation", "expense", "Wants"},

        // SAVINGS (20%)
        {"Emergency Fund", "expense", "Savings"},
        {"Investments", "expense", "Savings"}
    };

    int count = sizeof(defaults) / sizeof(defaults[0]);
    const char *sql = "INSERT INTO Categories (name, type, budget_bucket) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;

    // Prepare statement once
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[SEEDER](\"Failed to prepare seeder statement.\")");
        return 1;
    }

    // Loop through seeds
    for (int i = 0; i < count; i++) {
        sqlite3_bind_text(stmt, 1, defaults[i].name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, defaults[i].type, -1, SQLITE_STATIC);
        
        if (defaults[i].budget_bucket != NULL) {
            sqlite3_bind_text(stmt, 3, defaults[i].budget_bucket, -1, SQLITE_STATIC);
        } else {
            sqlite3_bind_null(stmt, 3);
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            // Optional: Print error for specific category failure or ignore duplicates
            fprintf(stderr, "Failed to insert category: %s\n", defaults[i].name);
            fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
            // fprintf(stderr, "Note: Category '%s' might already exist.\n", defaults[i].name);
        }

        sqlite3_reset(stmt); // Reset for next iteration
    }

    sqlite3_finalize(stmt);
    printf("Default categories created (Hardcoded).");

    return 0;
}