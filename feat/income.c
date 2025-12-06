#include <stdio.h>
#include <string.h>
#include "../sqlite/sqlite3.h"
#include "budget_rule.h"
#include "utils.h"
#include "wallet.h"

// function untuk memasukan income ke DB
int insert_income(sqlite3 *db, double amount, int userId, int wallet_id, char *wallet_name)
{

    printf("Inserting income: %.2f to wallet: %s\n", amount, wallet_name);

    char *sql = "INSERT INTO Transactions (amount, user_id, wallet_id, type) VALUES (?, ?, ?, 'income')";
    char *update_sql = "UPDATE Wallets SET balance = balance + ? WHERE id = ?";

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

// process untuk memasukan income ke db
int create_income(sqlite3 *db, int userId, int wallet_id, char *wallet_name)
{
    // coba ambil budget rule
    BudgetRule *rule = get_budget_rule_by_user_id(db, userId);

    double amount;

    // ambil amount
amount_input:
    amount = get_double_input("Enter amount of money you wanna add (input 0 to cancel/skip) ");

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

    // check untuk wallet kalo nggak ada di database untuk bisa bypass pemilihan wallet
    if (wallet_id > 0)
    {
        insert_income(db, amount, userId, wallet_id, wallet_name);
        return 0;
    }

    // check kalau tidak ada budget rule, langsung pilih wallet
    if (rule == NULL)
    {
        fprintf(stderr, "Budget rule not found for user ID %d.\n", userId);

        printf("Atempting to assign manually\n");

        Wallet **wallets = get_all_wallets_by_user_id(db, userId);

        if (wallets != NULL)
        {
            int i = 0;
            while (wallets[i] != NULL)
            {
                printf("%d. %s\n", i + 1, wallets[i]->name);
                i++;
            }

        wallet_selection_no_wallet:
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
                goto wallet_selection_no_wallet;
            }

            free_wallets(wallets);

            insert_income(db, amount, userId, selected_wallet_id, selected_wallet_name);
            return 0;
        }
        return 1;
    }

    // tanyakan apakah mau assign secara manual
    int yn_input = get_yes_or_no_input("Wanna assign income manually?", 0);

    // jika ya pilih wallet lalu masukan ke wallet tersebut
    if (yn_input)
    {
        Wallet **wallets = get_all_wallets_by_user_id(db, userId);

        if (wallets != NULL)
        {
            int i = 0;
            while (wallets[i] != NULL)
            {
                printf("%d. %s\n", i + 1, wallets[i]->name);
                i++;
            }

        wallet_selection:
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
                goto wallet_selection;
            }

            free_wallets(wallets);

            insert_income(db, amount, userId, selected_wallet_id, selected_wallet_name);
            return 0;
        }
        return 1;
    }

    // jika tidak alokasikan secara otomatis

    double needs_amount = (double)(amount * (rule->needs_percentage / 100.0));
    double wants_amount = (double)(amount * (rule->wants_percentage / 100.0));
    double savings_amount = (double)(amount * (rule->savings_percentage / 100.0));

    printf("Needs: %.2f\n", needs_amount);
    printf("Wants: %.2f\n", wants_amount);
    printf("Savings: %.2f\n", savings_amount);

    int confirm = get_yes_or_no_input("Confirm?", 1);

    if (!confirm)
    {
        printf("Canceled!\n");
        return 0;
    }

    Wallet **wallets = get_all_wallets_by_user_id_that_are_main(db, userId);

    int i = 0;
    int err = 0;
    while (wallets[i] != NULL)
    {
        printf("%d. %s\n", i + 1, wallets[i]->name);

        if(strcmp(wallets[i]->name, "Main") == 0)
        {
            err = insert_income(db, needs_amount, userId, wallets[i]->id, wallets[i]->name);
            if (err)
            {
                printf("Error: %s\n", sqlite3_errmsg(db));
                return 1;
            }
            
        }

        if (strcmp(wallets[i]->name, "Wants") == 0)
        {
            err = insert_income(db, wants_amount, userId, wallets[i]->id, wallets[i]->name);
            if (err)
            {
                printf("Error: %s\n", sqlite3_errmsg(db));
                return 1;
            }
        }

        if (strcmp(wallets[i]->name, "Savings") == 0)
        {
            err =insert_income(db, savings_amount, userId, wallets[i]->id, wallets[i]->name);
            if (err)
            {
                printf("Error: %s\n", sqlite3_errmsg(db));
                return 1;
            }
        }
        
        i++;
    }

    free_wallets(wallets);
    return 0;
}
