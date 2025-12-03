#include <stdio.h>
#include "../db/db.h"
#include <stdlib.h>
#include <string.h>
#include "wallet.h"
#include "budget_rule.h"
#include "utils.h"

void free_wallet(Wallet *wallet)
{
    if (wallet != NULL)
    {
        free(wallet);
    }
}

void free_wallets(Wallet **wallets)
{
    if (wallets == NULL)
    {
        return;
    }

    int i = 0;
    while (wallets[i] != NULL)
    {
        free_wallet(wallets[i]);
        i++;
    }

    free(wallets);
}

int create_wallet_first_time(sqlite3 *db, int userId)
{
    BudgetRule *rule = get_budget_rule_by_user_id(db, userId);
    if (rule == NULL)
    {
        fprintf(stderr, "Budget rule not found for user ID %d.\n", userId);
        exit(1);
    }

    int err;

    err = create_wallet(db, "Main", userId, rule->needs_percentage, 1);

    if (err == -1)
    {
        fprintf(stderr, "Failed to prepare create Main wallet: %s\n", sqlite3_errmsg(db));
        close_db(db);
        exit(1);
    }

    err = create_wallet(db, "Wants", userId, rule->wants_percentage, 1);

    if (err == -1)
    {
        fprintf(stderr, "Failed to prepare create Wants wallet: %s\n", sqlite3_errmsg(db));
        close_db(db);
        exit(1);
    }

    err = create_wallet(db, "Savings", userId, rule->savings_percentage, 1);

    if (err == -1)
    {
        fprintf(stderr, "Failed to prepare create Savings wallet: %s\n", sqlite3_errmsg(db));
        close_db(db);
        exit(1);
    }

    return 0;
}

int create_wallet(sqlite3 *db, char *walletName, int userId, int allocation, int is_main)
{

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO wallets (name, user_id, allocation_percentage, is_main) VALUES (?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, walletName, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, allocation);
    sqlite3_bind_int(stmt, 4, is_main);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    int wallet_id = sqlite3_last_insert_rowid(db);

    printf("Wallet '%s' created successfully.\n", walletName);

    sqlite3_finalize(stmt);

    return wallet_id;
};

long long create_wallet_return_id(sqlite3 *db, char *walletName, int userId, int allocation, int is_main)
{
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO wallets (name, user_id, allocation_percentage, is_main) VALUES (?, ?, ?, ?)";
    long long new_id = 0;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, walletName, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, allocation);
    sqlite3_bind_int(stmt, 4, is_main);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    printf("Wallet '%s' created successfully.\n", walletName);
    new_id = sqlite3_last_insert_rowid(db);

    sqlite3_finalize(stmt);

    return new_id;
};

// Wallet *get_wallets_by_user_id(sqlite3 *db, int userId)
// {
//     sqlite3_stmt *stmt;
//     const char *sql = "SELECT id, user_id, name, balance, allocation_percentage FROM wallets WHERE user_id = ?";

//     int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

//     if (rc != SQLITE_OK)
//     {
//         fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
//         exit(1);
//     }

//     sqlite3_bind_int(stmt, 1, userId);

//     rc = sqlite3_step(stmt);

//     if (rc == SQLITE_ROW)
//     {
//         Wallet *wallet = malloc(sizeof(Wallet));
//         wallet->id = sqlite3_column_int(stmt, 0);
//         wallet->user_id = sqlite3_column_int(stmt, 1);
//         strcpy(wallet->name, (char *)sqlite3_column_text(stmt, 2));
//         wallet->balance = sqlite3_column_double(stmt, 3);
//         wallet->allocation = sqlite3_column_int(stmt, 4);

//         sqlite3_finalize(stmt);

//         return wallet;
//     }
//     else
//     {
//         sqlite3_finalize(stmt);
//         return NULL;
//     }
// }

void show_wallet(Wallet **wallets)
{

    if (wallets == NULL)
    {
        fprintf(stderr, "No wallets found.\n");
        return;
    }

    int count = 0;

    int widths[] = {4, 7, 20, 15, 8, 5};
    int num_cols = 6;

    printf("\n--- Wallets Table ---\n");
    print_line(widths, num_cols);

    printf("| %-*s | %-*s | %-*s | %-*s | %-*s | %-*s |\n",
           widths[0], "ID",
           widths[1], "User ID",
           widths[2], "Name",
           widths[3], "Balance",
           widths[4], "Alloc %",
           widths[5], "Main");

    print_line(widths, num_cols);

    while (wallets[count] != NULL)
    {
        printf("| %-*d | %-*d | %-*s | %-*f | %-*d | %-*s |\n",
               widths[0], wallets[count]->id,
               widths[1], wallets[count]->user_id,
               widths[2], wallets[count]->name,
               widths[3], wallets[count]->balance,
               widths[4], wallets[count]->allocation,
               widths[5], wallets[count]->is_main ? "Yes" : "No");

        count++;
    }

    print_line(widths, num_cols);

    getchar();
}

Wallet **get_all_wallets_by_user_id(sqlite3 *db, int userId)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, user_id, name, balance, allocation_percentage FROM wallets WHERE user_id = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_bind_int(stmt, 1, userId);

    Wallet **wallets = NULL;
    int count = 0;

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:

            Wallet **temp = realloc(wallets, sizeof(Wallet *) * (count + 1));

            if (temp == NULL)
            {
                fprintf(stderr, "Memory allocation failed for array\n");
                free(wallets);
                exit(1);
            }

            wallets = temp;

            wallets[count] = (Wallet *)malloc(sizeof(Wallet));

            if (wallets[count] == NULL)
            {
                fprintf(stderr, "Memory allocation failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                exit(1);
            }

            wallets[count]->id = sqlite3_column_int(stmt, 0);
            wallets[count]->user_id = sqlite3_column_int(stmt, 1);

            const unsigned char *text = sqlite3_column_text(stmt, 2);
            if (text)
            {
                strncpy(wallets[count]->name, (const char *)text, sizeof(wallets[count]->name) - 1);
                wallets[count]->name[sizeof(wallets[count]->name) - 1] = '\0';
            }
            else
            {
                wallets[count]->name[0] = '\0';
            }

            wallets[count]->balance = sqlite3_column_double(stmt, 3);
            wallets[count]->allocation = sqlite3_column_int(stmt, 4);

            count++;
            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);

    Wallet **temp = realloc(wallets, sizeof(Wallet *) * (count + 1));
    if (temp)
    {
        wallets = temp;
        wallets[count] = NULL;
    }

    return wallets;
}

Wallet **get_all_wallets_by_user_id_that_are_main(sqlite3 *db, int userId)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, user_id, name, balance, allocation_percentage FROM wallets WHERE user_id = ? AND is_main = 1";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_bind_int(stmt, 1, userId);

    Wallet **wallets = NULL;
    int count = 0;

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:

            Wallet **temp = realloc(wallets, sizeof(Wallet *) * (count + 1));

            if (temp == NULL)
            {
                fprintf(stderr, "Memory allocation failed for array\n");
                free(wallets);
                exit(1);
            }

            wallets = temp;

            wallets[count] = (Wallet *)malloc(sizeof(Wallet));

            if (wallets[count] == NULL)
            {
                fprintf(stderr, "Memory allocation failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                exit(1);
            }

            wallets[count]->id = sqlite3_column_int(stmt, 0);
            wallets[count]->user_id = sqlite3_column_int(stmt, 1);

            const unsigned char *text = sqlite3_column_text(stmt, 2);
            if (text)
            {
                strncpy(wallets[count]->name, (const char *)text, sizeof(wallets[count]->name) - 1);
                wallets[count]->name[sizeof(wallets[count]->name) - 1] = '\0';
            }
            else
            {
                wallets[count]->name[0] = '\0';
            }

            wallets[count]->balance = sqlite3_column_double(stmt, 3);
            wallets[count]->allocation = sqlite3_column_int(stmt, 4);

            count++;
            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);

    Wallet **temp = realloc(wallets, sizeof(Wallet *) * (count + 1));
    if (temp)
    {
        wallets = temp;
        wallets[count] = NULL;
    }

    return wallets;
}