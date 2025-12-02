#include <stdio.h>
#include "migrations.h"

typedef struct
{
    int id;
    char* up;
    char* down;
} Migration;

static Migration registry[] = {
    {
        1,
        "CREATE TABLE Users ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        username TEXT NOT NULL UNIQUE, \
        password_hash TEXT NOT NULL, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP \
        );",
        "DROP TABLE IF EXISTS Users;" 
    },

    {
        2,
        "CREATE TABLE Wallets ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        name TEXT NOT NULL, \
        balance DECIMAL(15, 2) NOT NULL DEFAULT 0, \
        allocation_percentage INTEGER DEFAULT 0, \
        is_main BOOLEAN NOT NULL DEFAULT 0, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE \
        ); \
        CREATE INDEX idx_wallets_user_id ON Wallets(user_id);",
        "DROP TABLE IF EXISTS Wallets;" 
    },

    {
        3,
        "CREATE TABLE Categories ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        name TEXT NOT NULL, \
        type TEXT NOT NULL CHECK(type IN ('income', 'expense')), \
        budget_bucket TEXT CHECK(budget_bucket IN ('Needs', 'Wants', 'Savings')), \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE \
        ); \
        CREATE UNIQUE INDEX idx_categories_unique_user ON Categories(user_id, name, type);",
        "DROP TABLE IF EXISTS Categories;" 
    },

    {
        4,
        "CREATE TABLE Debts ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        name TEXT NOT NULL, \
        total_amount DECIMAL(15, 2) NOT NULL, \
        amount_paid DECIMAL(15, 2) NOT NULL DEFAULT 0, \
        is_installment BOOLEAN NOT NULL DEFAULT 0, \
        monthly_payment DECIMAL(15, 2), \
        payment_due_day INTEGER, \
        due_date DATE, \
        is_paid BOOLEAN NOT NULL DEFAULT 0, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE \
        ); \
        CREATE INDEX idx_debts_user_id ON Debts(user_id);",
        "DROP TABLE IF EXISTS Debts;" 
    },

    {
        5,
        "CREATE TABLE SavingsGoals ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        name TEXT NOT NULL, \
        target_amount DECIMAL(15, 2) NOT NULL, \
        amount_saved DECIMAL(15, 2) NOT NULL DEFAULT 0, \
        target_date DATE, \
        is_achieved BOOLEAN NOT NULL DEFAULT 0, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE \
        ); \
        CREATE INDEX idx_savings_goals_user_id ON SavingsGoals(user_id);",
        "DROP TABLE IF EXISTS SavingsGoals;" 
    },

    {
        6,
        "CREATE TABLE Transactions ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        wallet_id INTEGER NOT NULL, \
        category_id INTEGER, \
        debt_id INTEGER, \
        savings_goal_id INTEGER, \
        type TEXT NOT NULL CHECK(type IN ('income', 'expense', 'send', 'accept')), \
        amount DECIMAL(15, 2) NOT NULL, \
        description TEXT, \
        transaction_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE, \
        FOREIGN KEY (wallet_id) REFERENCES Wallets(id) ON DELETE RESTRICT, \
        FOREIGN KEY (category_id) REFERENCES Categories(id) ON DELETE SET NULL, \
        FOREIGN KEY (debt_id) REFERENCES Debts(id) ON DELETE SET NULL, \
        FOREIGN KEY (savings_goal_id) REFERENCES SavingsGoals(id) ON DELETE SET NULL \
        ); \
        CREATE INDEX idx_transactions_user_id ON Transactions(user_id); \
        CREATE INDEX idx_transactions_wallet_id ON Transactions(wallet_id); \
        CREATE INDEX idx_transactions_category_id ON Transactions(category_id); \
        CREATE INDEX idx_transactions_debt_id ON Transactions(debt_id); \
        CREATE INDEX idx_transactions_savings_goal_id ON Transactions(savings_goal_id); \
        CREATE INDEX idx_transactions_date ON Transactions(transaction_date);",
        "DROP TABLE IF EXISTS Transactions;" 
    },

    {
        7,
        "CREATE TABLE BudgetRules ( \
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user_id INTEGER NOT NULL, \
        needs_percentage INTEGER NOT NULL DEFAULT 50, \
        wants_percentage INTEGER NOT NULL DEFAULT 30, \
        savings_percentage INTEGER NOT NULL DEFAULT 20, \
        target_income DECIMAL(15, 2) NOT NULL, \
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE \
        ); \
        CREATE INDEX idx_budget_rules_user_id ON BudgetRules(user_id);",
        "DROP TABLE IF EXISTS BudgetRules;" 
    }
};

#define MIGRATION_COUNT 7 //DONT FORGET TO COUNT EVERYTIME YOU ADD MIGRATIONS

int ensure_migration_table(sqlite3 *db) {
     const char* sql = "CREATE TABLE IF NOT EXISTS _migrations ("
                      "id INTEGER PRIMARY KEY, "
                      "applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    return sqlite3_exec(db, sql, 0, 0, 0);
}

int get_current_version(sqlite3* db) {
    sqlite3_stmt* stmt;
    int version = 0;
    const char* sql = "SELECT MAX(id) FROM _migrations";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            version = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return version;
}

int migrate_up(sqlite3* db) {
    if (ensure_migration_table(db) != SQLITE_OK) return SQLITE_ERROR;

    int current_ver = get_current_version(db);
    printf("[UP] Current DB Version: %d\n", current_ver);

    for (int i = 0; i < MIGRATION_COUNT; i++) {
        Migration m = registry[i];

        if (m.id > current_ver) {
            printf("Applying Migration %d (UP)...\n", m.id);
            
            char* errMsg = 0;
            int rc = sqlite3_exec(db, m.up, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Migration %d UP Failed: %s\n", m.id, errMsg);
                sqlite3_free(errMsg);
                return rc;
            }

            // Record success
            char record_sql[64];
            sprintf(record_sql, "INSERT INTO _migrations (id) VALUES (%d);", m.id);
            sqlite3_exec(db, record_sql, 0, 0, 0);
            printf("Migration %d applied.\n", m.id);
        }
    }
    return SQLITE_OK;
}

int migrate_down(sqlite3* db) {
    if (ensure_migration_table(db) != SQLITE_OK) return SQLITE_ERROR;

    int current_ver = get_current_version(db);
    printf("[DOWN] Current DB Version: %d\n", current_ver);

    if (current_ver == 0) {
        printf("No migrations to rollback.\n");
        return SQLITE_OK;
    }

    // Find the migration definition for the current version
    // (We loop backwards or just search for the matching ID)
    for (int i = 0; i < MIGRATION_COUNT; i++) {
        Migration m = registry[i];
        
        if (m.id == current_ver) {
            printf("Rolling back Migration %d (DOWN)...\n", m.id);
            
            char* errMsg = 0;
            int rc = sqlite3_exec(db, m.down, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Migration %d DOWN Failed: %s\n", m.id, errMsg);
                sqlite3_free(errMsg);
                return rc;
            }

            // Remove record
            char delete_sql[64];
            sprintf(delete_sql, "DELETE FROM _migrations WHERE id = %d;", m.id);
            sqlite3_exec(db, delete_sql, 0, 0, 0);
            
            printf("Migration %d rolled back.\n", m.id);
            return SQLITE_OK; // Only rollback one at a time
        }
    }
    
    // If we found a version ID in the DB that isn't in our code registry
    fprintf(stderr, "Error: Version %d found in DB but not in code registry.\n", current_ver);
    return SQLITE_ERROR;
}

