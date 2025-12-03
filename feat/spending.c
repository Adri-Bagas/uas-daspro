#include <stdio.h>
#include <string.h>
#include "../sqlite/sqlite3.h"
#include "budget_rule.h"
#include "utils.h"
#include "category.h"
#include "wallet.h"

/**
 * Insert a spending transaction into the database.
 *
 * @param db The database connection
 * @param amount The amount of the spending
 * @param userId The user ID of the spending
 * @param wallet_id The wallet ID of the spending
 * @param wallet_name The name of the wallet of the spending
 * @param category_id The category ID of the spending
 *
 * @return 0 on success, 1 on failure
 */
int insert_spending(sqlite3 *db, double amount, int userId, int wallet_id, char *wallet_name, int category_id)
{

    printf("Inserting spending: %.2f from wallet: %s\n", amount, wallet_name);

    char *sql = "INSERT INTO Transactions (amount, user_id, wallet_id, type, category_id) VALUES (?, ?, ?, 'spending')";
    char *update_sql = "UPDATE Wallets SET balance = balance - ? WHERE id = ?";

    sqlite3_stmt *stmt;
    sqlite3_stmt *stmt2;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, wallet_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);

    rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt2, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_double(stmt2, 1, amount);
    sqlite3_bind_int(stmt2, 2, wallet_id);

    rc = sqlite3_step(stmt2);

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt2);
        return 1;
    }

    sqlite3_finalize(stmt2);

    return 0;
}

int create_spending(sqlite3 *db, int userId, int wallet_id, char *wallet_name, int category_id)
{
    double amount;

amount_input:
    amount = get_double_input("Enter amount of money you wanna spend (input 0 to cancel/skip) ");

    if (amount == 0)
    {
        printf("Canceled!\n");
        return 0;
    }

    if (amount < 0)
    {
        printf("Invalid amount!\n");
        goto amount_input;
    }

    int selected_category_id = category_id;

    if (selected_category_id < 1)
    {
        Category **category = get_all_categories(db);

        if (category != NULL)
        {
            int i = 0;
            while (category[i] != NULL)
            {
                printf("%d. %s\n", i + 1, category[i]->name);
                i++;
            }

        category_selection_spending:
            int category_input = get_int_input("Select category (input 0 to cancel): ");

            if (category_input == 0)
            {
                printf("Canceled!\n");
                free_categories(category);
                return 0;
            }

            if (category_input > 0 && category_input <= i)
            {
                selected_category_id = category[category_input - 1]->id;
            }
            else
            {
                printf("Invalid input\n");
                goto category_selection_spending;
            }

            free_categories(category);
        }

        // check untuk wallet kalo ada di database validasi nya jika lebih dari 1 maka pasti ada, kalaupun tidak ada akan di handle saat insert
        if (wallet_id > 0)
        {
            insert_spending(db, amount, userId, wallet_id, wallet_name, selected_category_id);
            return 0;
        }

        Wallet **wallets = get_all_wallets_by_user_id(db, userId);

        if (wallets != NULL)
        {
            int i = 0;
            while (wallets[i] != NULL)
            {
                printf("%d. %s\n", i + 1, wallets[i]->name);
                i++;
            }

        wallet_selection_spending:
            int wallet_input = get_int_input("Select wallet (input 0 to cancel): ");

            if (wallet_input == 0)
            {
                printf("Canceled!\n");
                free_wallets(wallets);
                return 0;
            }

            int selected_wallet_id = 0;
            char selected_wallet_name[50] = "";

            if (wallet_input > 0 && wallet_input <= i)
            {
                selected_wallet_id = wallets[wallet_input - 1]->id;

                strcpy(selected_wallet_name, wallets[wallet_input - 1]->name);
            }
            else
            {
                printf("Invalid input\n");
                goto wallet_selection_spending;
            }

            free_wallets(wallets);

            insert_spending(db, amount, userId, selected_wallet_id, selected_wallet_name, selected_category_id);
            return 0;
        }
        return 1;
    }
}