#include "transaction.h"

void free_transaction(Transaction *transaction) {
    if (transaction == NULL) return;

    free(transaction->type);
    free(transaction->description);
    free(transaction->date);
    free(transaction->wallet_name);
    free(transaction->category_name);
    free(transaction);
}

void free_transactions(Transaction **transactions) {
    if (transactions == NULL) return;

    int i = 0;
    while (transactions[i] != NULL) {
        free_transaction(transactions[i]);
        i++;
    }
    free(transactions); // Free the list
}

// Helper function to process rows (internal use)
Transaction **fetch_transactions_from_stmt(sqlite3_stmt *stmt) {
    Transaction **transactions = NULL;
    int count = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction **temp = realloc(transactions, sizeof(Transaction*) * (count + 2));
        if (!temp) {
            // Handle memory error
            return transactions;
        }
        transactions = temp;

        transactions[count] = malloc(sizeof(Transaction));

        // Populate basic fields
        transactions[count]->id = sqlite3_column_int(stmt, 0);
        transactions[count]->user_id = sqlite3_column_int(stmt, 1);
        transactions[count]->wallet_id = sqlite3_column_int(stmt, 2);
        transactions[count]->category_id = sqlite3_column_int(stmt, 3);
        
        // Strings
        const char *txt_type = (const char *)sqlite3_column_text(stmt, 4);
        transactions[count]->type = txt_type ? strdup(txt_type) : strdup("unknown");

        transactions[count]->amount = sqlite3_column_double(stmt, 5);

        const char *txt_desc = (const char *)sqlite3_column_text(stmt, 6);
        transactions[count]->description = txt_desc ? strdup(txt_desc) : strdup("");

        const char *txt_date = (const char *)sqlite3_column_text(stmt, 7);
        transactions[count]->date = txt_date ? strdup(txt_date) : strdup("");

        // Joins
        const char *txt_w_name = (const char *)sqlite3_column_text(stmt, 8);
        transactions[count]->wallet_name = txt_w_name ? strdup(txt_w_name) : strdup("Unknown");

        const char *txt_c_name = (const char *)sqlite3_column_text(stmt, 9);
        transactions[count]->category_name = txt_c_name ? strdup(txt_c_name) : strdup("-");

        count++;
    }

    if (transactions != NULL) {
        transactions[count] = NULL;
    } else {
        return NULL;
    }

    return transactions;
}


Transaction **get_transactions_by_date_range(sqlite3 *db, int user_id, const char *start_date, const char *end_date) {
    sqlite3_stmt *stmt;
    
    // Uses the date() function to ignore time components if present
    const char *sql = 
        "SELECT t.id, t.user_id, t.wallet_id, t.category_id, "
        "t.type, t.amount, t.description, t.transaction_date, "
        "w.name AS wallet_name, c.name AS category_name "
        "FROM Transactions t "
        "JOIN Wallets w ON t.wallet_id = w.id "
        "LEFT JOIN Categories c ON t.category_id = c.id "
        "WHERE t.user_id = ? "
        "AND date(t.transaction_date) BETWEEN date(?) AND date(?) "
        "ORDER BY t.transaction_date DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end_date, -1, SQLITE_STATIC);

    Transaction **result = fetch_transactions_from_stmt(stmt);
    
    sqlite3_finalize(stmt);
    return result;
}

Transaction **get_transactions_by_month_year(sqlite3 *db, int user_id, int month, int year) {
    sqlite3_stmt *stmt;
    char date_pattern[8]; // Buffer to hold "YYYY-MM"

    // Format the month/year into a string like "2023-11"
    // %04d ensures 2023, %02d ensures 05 (adds leading zero)
    snprintf(date_pattern, sizeof(date_pattern), "%04d-%02d", year, month);

    const char *sql = 
        "SELECT t.id, t.user_id, t.wallet_id, t.category_id, "
        "t.type, t.amount, t.description, t.transaction_date, "
        "w.name AS wallet_name, c.name AS category_name "
        "FROM Transactions t "
        "JOIN Wallets w ON t.wallet_id = w.id "
        "LEFT JOIN Categories c ON t.category_id = c.id "
        "WHERE t.user_id = ? "
        "AND strftime('%Y-%m', t.transaction_date) = ? "
        "ORDER BY t.transaction_date DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    
    // IMPORTANT: Use SQLITE_TRANSIENT because 'date_pattern' is a local variable
    // and will be destroyed when this function ends. SQLite needs to make its own copy.
    sqlite3_bind_text(stmt, 2, date_pattern, -1, SQLITE_TRANSIENT);

    Transaction **result = fetch_transactions_from_stmt(stmt);
    
    sqlite3_finalize(stmt);
    return result;
}

Transaction **get_all_transactions_by_user_id_with_limit(sqlite3 *db, int user_id, int limit) {
    sqlite3_stmt *stmt;
    
    // query dengan join untuk memudahkan pengambilan data
    const char *sql = 
        "SELECT t.id, t.user_id, t.wallet_id, t.category_id, "
        "t.type, t.amount, t.description, t.transaction_date, "
        "w.name AS wallet_name, c.name AS category_name "
        "FROM Transactions t "
        "JOIN Wallets w ON t.wallet_id = w.id "
        "LEFT JOIN Categories c ON t.category_id = c.id "
        "WHERE t.user_id = ? "
        "ORDER BY t.transaction_date DESC "
        "LIMIT ? ;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    int _limit = 10;

    if (limit > 0)
    {
        _limit = limit;
    }
    
    sqlite3_bind_int(stmt, 2, _limit);

    Transaction **transactions = fetch_transactions_from_stmt(stmt);
    
    sqlite3_finalize(stmt);
    return transactions;
}


Transaction **get_all_transactions_by_user_id(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    
    // query dengan join untuk memudahkan pengambilan data
    const char *sql = 
        "SELECT t.id, t.user_id, t.wallet_id, t.category_id, "
        "t.type, t.amount, t.description, t.transaction_date, "
        "w.name AS wallet_name, c.name AS category_name "
        "FROM Transactions t "
        "JOIN Wallets w ON t.wallet_id = w.id "
        "LEFT JOIN Categories c ON t.category_id = c.id "
        "WHERE t.user_id = ? "
        "ORDER BY t.transaction_date DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    Transaction **transactions = fetch_transactions_from_stmt(stmt);
    
    sqlite3_finalize(stmt);
    return transactions;
}

void show_transactions_table(Transaction **transactions) {
    if (transactions == NULL) {
        printf("No transactions found.\n");
        return;
    }

    // Widths: Date, Type, Wallet, Category, Amount
    int w[] = {19, 8, 15, 26, 12}; 
    int cols = 5;

    printf("\n--- Transaction History ---\n");
    print_line(w, cols);
    printf("| %-*s | %-*s | %-*s | %-*s | %-*s |\n",
           w[0], "Date",
           w[1], "Type",
           w[2], "Wallet",
           w[3], "Category",
           w[4], "Amount");
    print_line(w, cols);

    int i = 0;
    while (transactions[i] != NULL) {
        Transaction *t = transactions[i];

        // Truncate description kalo terlalu panjang buat tabel
        char short_cat[26]; 
        snprintf(short_cat, 26, "%s", t->category_name);

        double amount = t->amount;

        if (amount < 0) {
            amount = -amount;
        }

        printf("| %-*s | %-*s | %-*s | %-*s | %*.2f |\n",
               w[0], t->date,
               w[1], t->type,
               w[2], t->wallet_name,
               w[3], short_cat,
               w[4], amount 
            );
        i++;
    }
    print_line(w, cols);
}
