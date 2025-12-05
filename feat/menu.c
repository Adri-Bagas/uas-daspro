#include "menu.h"
#include "auth.h"
#include "utils.h"
#include "user.h"
#include "wallet.h"
#include "budget_rule.h"
#include "income.h"
#include "spending.h"
#include "transaction.h"
#include "category.h"
#include "calculator.h"
#include "generate.h"
#ifdef _WIN32
#include <windows.h>
#define SLEEP_SECONDS(seconds) Sleep(seconds * 1000)
#else

#include <unistd.h>
#define SLEEP_SECONDS(seconds) sleep(seconds)
#endif

void handle_wallet_menu(sqlite3 *db, int user_id)
{
    // We declare the pointer here, but we load data INSIDE the loop
    Wallet **wallets = NULL;
    char choice[5];

    while (1)
    {
        // 1. REFRESH DATA: Load fresh data at the start of every loop iteration
        wallets = get_all_wallets_by_user_id(db, user_id);

        clear_screen();

        // 2. DISPLAY: Show the table
        if (wallets != NULL)
        {
            show_wallet(wallets);
            int total_saves = 0;

            for (size_t i = 0; i < (sizeof(wallets) / sizeof(wallets[0])); i++)
            {
                total_saves += wallets[i]->balance; 
            }

            printf("Total Savings: %d\n", total_saves);
        }
        else
        {
            printf("No wallets found.\n");
        }

        printf("\nWALLETS MENU\n");
        printf("----------------\n");
        printf("1. Add Wallet\n");
        printf("2. Delete Wallet\n");
        printf("3. Transfer Funds\n");
        printf("4. Return\n");
        printf("----------------\n");
        printf("Select option (1-4): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL)
            break;

        choice[strcspn(choice, "\n")] = 0;

        // --- OPTION 1: ADD ---
        if (strcmp(choice, "1") == 0)
        {
            // clear_screen(); // Optional, feels snappier without it here
            printf("\n--- Create Wallet ---\n");
            printf("Enter wallet name: ");

            char wallet_name[64];
            fgets(wallet_name, 64, stdin);
            wallet_name[strcspn(wallet_name, "\n")] = 0;

            int wallet_id = create_wallet(db, wallet_name, user_id, 0, 0);

            if (wallet_id != -1)
                printf(">> Wallet created successfully (ID: %d)\n", wallet_id);
            else
                printf(">> Failed to create wallet.\n");

            printf("Press enter to continue...");
            getchar();
        }

        // --- OPTION 2: DELETE ---
        else if (strcmp(choice, "2") == 0)
        {
            if (wallets == NULL)
            {
                printf("No wallets to delete.\nPress enter...");
                getchar();
            }
            else
            {
                int i = 0;
                printf("\n--- Delete Wallet ---\n");
                while (wallets[i] != NULL)
                {
                    printf("%d. %s\n", i + 1, wallets[i]->name);
                    i++;
                }

                int wallet_input = get_int_input("Select wallet (input 0 to cancel): ");

                if (wallet_input > 0 && wallet_input <= i)
                {
                    Wallet *selected = wallets[wallet_input - 1];
                    if (delete_wallet(db, user_id, selected) == 0)
                    {
                        printf(">> Wallet deleted successfully.\n");
                    }
                    else
                    {
                        printf(">> Failed to delete wallet.\n");
                    }
                }
                else if (wallet_input != 0)
                {
                    printf("Invalid input.\n");
                }

                if (wallet_input != 0)
                { // Only pause if we didn't cancel immediately
                    printf("Press enter to continue...");
                    getchar();
                }
            }
        }

        // --- OPTION 3: TRANSFER ---
        else if (strcmp(choice, "3") == 0)
        {
            if (wallets == NULL || wallets[0] == NULL || wallets[1] == NULL)
            { // Need at least 2 wallets
                printf("Need at least 2 wallets to transfer.\nPress enter...");
                getchar();
            }
            else
            {
                clear_screen();

                Wallet *from_wallet = NULL;
                Wallet *to_wallet = NULL;

                int wallet_input;
                double amount;

                if (wallets != NULL)
                {
                    int i = 0;
                    while (wallets[i] != NULL)
                    {
                        printf("%d. %s\n", i + 1, wallets[i]->name);
                        i++;
                    }

                from_wallet_selection:
                    wallet_input = get_int_input("Select wallet you wanna get money from! (input 0 to cancel): ");

                    if (wallet_input == 0)
                    {
                        printf("Canceled!\n");
                        goto exit_transfer;
                    }

                    if (wallet_input > 0 && wallet_input <= i)
                    {
                        from_wallet = wallets[wallet_input - 1];
                    }
                    else
                    {
                        printf("Invalid input\n");
                        goto from_wallet_selection;
                    }
                }

                if (wallets != NULL)
                {
                    int i = 0;
                    while (wallets[i] != NULL)
                    {
                        if (wallets[i]->id == from_wallet->id)
                        {
                            i++;
                            continue;
                        }
                        printf("%d. %s\n", i + 1, wallets[i]->name);
                        i++;
                    }

                to_wallet_selection:
                    wallet_input = get_int_input("Select wallet you wanna put the money! (input 0 to cancel): ");

                    if (wallet_input == 0)
                    {
                        printf("Canceled!\n");
                        free_wallets(wallets);
                        goto exit_transfer;
                    }

                    if (wallet_input > 0 && wallet_input <= i)
                    {
                        to_wallet = wallets[wallet_input - 1];
                    }
                    else
                    {
                        printf("Invalid input\n");
                        goto to_wallet_selection;
                    }
                }

                if (from_wallet == NULL || to_wallet == NULL)
                {
                    printf("Wallet not selected\n");
                    goto from_wallet_selection;
                }

            amount_input:

                amount = get_double_input("Enter amount of money you wanna transfer (input 0 to cancel) ");

                if (amount == 0)
                {
                    printf("Canceled!\n");
                    goto exit_transfer;
                }

                if (amount < 0)
                {
                    printf("Invalid amount!\n");
                    goto amount_input;
                }

                int err;
                err = transfer_funds(db, user_id, from_wallet, to_wallet, amount);

                if (err)
                {
                    printf("Failed to transfer funds\n");
                    fprintf(stderr, "Failed to transfer funds: %s\n", sqlite3_errmsg(db));
                }
                else
                {
                    printf("Funds transfered successfully\n");
                }

                free_wallets(wallets);
                wallets = NULL;

            exit_transfer:
                printf("Press enter to continue...");
                getchar();
            }
        }

        // --- OPTION 4: RETURN ---
        else if (strcmp(choice, "4") == 0)
        {
            // This is the ONLY time we break the loop
            break;
        }

        // CLEANUP: Free memory at the end of the loop
        //  so we can fetch fresh data in the next iteration
        if (wallets != NULL)
        {
            free_wallets(wallets);
            wallets = NULL;
        }
    }

    // Final cleanup in case we broke out of loop with memory allocated
    if (wallets != NULL)
    {
        free_wallets(wallets);
    }
}
void handle_income_menu(sqlite3 *db, int user_id)
{
    create_income(db, user_id, 0, NULL);
}

void handle_spending_menu(sqlite3 *db, int user_id)
{
    create_spending(db, user_id, 0, NULL, 0);
}

void handle_history_menu(sqlite3 *db, int user_id)
{
    Transaction **transactions = NULL;
    char choice[5];

    while (1)
    {
        clear_screen();
        transactions = get_all_transactions_by_user_id(db, user_id);

        if (transactions == NULL)
        {
            printf("Failed to fetch transactions\n");
            break;
        }

        show_transactions_table(transactions);

        printf("1. Filter by date range\n");
        printf("2. Filter by month and year\n");
        printf("3. Back\n");
        printf("Select option (1-3): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL)
            continue;

        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "1") == 0)
        {
            printf("Enter the starting date (YYYY-MM-DD): ");
            char start_date[11];
            if (fgets(start_date, sizeof(start_date), stdin) == NULL)
                continue;
            start_date[strcspn(start_date, "\n")] = 0;

            clear_input_buffer();

            int len = strlen(start_date);

            if (strlen(start_date) != 10 || start_date[4] != '-' || start_date[7] != '-')
            {
                printf("Invalid date format\n");
                printf("Pressed enter to continue...\n");
                getchar();
                continue;
            }

            printf("Enter the ending date (YYYY-MM-DD): ");
            char end_date[11];
            if (fgets(end_date, sizeof(end_date), stdin) == NULL)
                continue;
            end_date[strcspn(end_date, "\n")] = 0;

            clear_input_buffer();

            if (strlen(end_date) != 10 || end_date[4] != '-' || end_date[7] != '-')
            {
                printf("Invalid date format\n");
                printf("Pressed enter to continue...\n");
                getchar();
                continue;
            }

            Transaction **filtered = get_transactions_by_date_range(db, user_id, start_date, end_date);

            if (filtered == NULL)
            {
                printf("Failed to fetch transactions\n");
            }

            if (filtered != NULL)
            {
                show_transactions_table(filtered);
            }
            show_transactions_table(filtered);

            free_transactions(filtered);
            filtered = NULL;

            printf("Press enter to continue...");
            getchar();
        }
        else if (strcmp(choice, "2") == 0)
        {
            printf("Enter the month (1-12): ");
            char month[3];
            if (fgets(month, sizeof(month), stdin) == NULL)
                continue;
            month[strcspn(month, "\n")] = 0;

            clear_input_buffer();

            printf("Enter the year: ");
            char year[5];
            if (fgets(year, sizeof(year), stdin) == NULL)
                continue;
            year[strcspn(year, "\n")] = 0;

            clear_input_buffer();

            Transaction **filtered = get_transactions_by_month_year(db, user_id, atoi(month), atoi(year));

            if (filtered == NULL)
            {
                fprintf(stderr, "Failed to fetch transactions\n");
                printf("Pressed enter to continue...\n");
                getchar();
                goto exit_history_menu;
            }

            show_transactions_table(filtered);

            free_transactions(filtered);
            filtered = NULL;

            printf("Press enter to continue...");
            getchar();
        }
        else if (strcmp(choice, "3") == 0)
        {
            break;
        }
        else
        {
            printf("Invalid option\n");
        }

        if (transactions != NULL)
        {
            free_transactions(transactions);
            transactions = NULL;
        }
    }

exit_history_menu:
    if (transactions != NULL)
    {
        free_transactions(transactions);
        transactions = NULL;
    }
}

void handle_calculator_menu()
{
    show_calculator_menu();
}

void handle_report_menu(sqlite3 *db, User *user)
{

    char month[3];
    char year[5];

    while (1)
    {
        printf("Enter the month (1-12): ");

        if (fgets(month, sizeof(month), stdin) == NULL)
            continue;
        month[strcspn(month, "\n")] = 0;

        clear_input_buffer();
        int m = atoi(month);
        if (m >= 1 && m <= 12) break;
        printf("Invalid month. Please enter 1-12.\n");
    }

    while (1)
    {
        printf("Enter the year: ");

        if (fgets(year, sizeof(year), stdin) == NULL)
            continue;
        year[strcspn(year, "\n")] = 0;

        clear_input_buffer();
        int y = atoi(year);
        if (y >= 1900 && y <= 3000) break;
        printf("Invalid year.\n");
    }

    generate_report(db, user, atoi(month), atoi(year));
}

void handle_help_menu() {
    char choice[5];

    while (1) {
        clear_screen();
        printf("========================================\n");
        printf("          HELP & DOCUMENTATION          \n");
        printf("========================================\n");
        printf("1. Wallets & Transfers\n");
        printf("2. Income & Spending\n");
        printf("3. Transaction History\n");
        printf("4. Reports & Calculator\n");
        printf("5. Return to Main Menu\n");
        printf("----------------------------------------\n");
        printf("Select a topic (1-5): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL) break;
        choice[strcspn(choice, "\n")] = 0;

        // --- TOPIC 1: WALLETS ---
        if (strcmp(choice, "1") == 0) {
            clear_screen();
            printf("[ WALLETS & TRANSFERS ]\n\n");
            printf("1. MANAGING WALLETS:\n");
            printf("   - Before recording transactions, you must create a Wallet.\n");
            printf("   - Wallets represent where your money is (e.g., 'Cash', 'Bank BCA', 'E-Wallet').\n");
            printf("   - You can delete wallets, but be careful as this might affect history. main wallet cannot be deleted\n\n");
            
            printf("2. TRANSFER FUNDS:\n");
            printf("   - Use this to move money between two existing wallets.\n");
            printf("   - Example: Moving money from 'Bank' to 'Cash' (ATM Withdrawal).\n");
            printf("   - You must have at least 2 wallets created to use this feature.\n");
            
            printf("Pressed enter to continue...\n");
            getchar();
        }

        // --- TOPIC 2: INCOME & SPENDING ---
        else if (strcmp(choice, "2") == 0) {
            clear_screen();
            printf("[ INCOME & SPENDING ]\n\n");
            printf("1. REGISTER INCOME:\n");
            printf("   - Use this when you receive money (Salary, Bonus, Gift).\n");
            printf("   - You will select a Wallet to deposit the money into.\n");
            printf("   - This INCREASES the balance of the selected wallet.\n\n");

            printf("2. REGISTER SPENDING:\n");
            printf("   - Use this when you make a purchase.\n");
            printf("   - You will select a Wallet to pay from.\n");
            printf("   - This DECREASES the balance of the selected wallet.\n");
            printf("   - Ensure the wallet has enough funds before spending!\n");

            printf("Pressed enter to continue...\n");
            getchar();
        }

        // --- TOPIC 3: HISTORY ---
        else if (strcmp(choice, "3") == 0) {
            clear_screen();
            printf("[ TRANSACTION HISTORY ]\n\n");
            printf("This menu allows you to view your past financial activities.\n\n");
            printf("FILTER OPTIONS:\n");
            printf("1. By Date Range:\n");
            printf("   - View transactions between two specific dates.\n");
            printf("   - Format must be YYYY-MM-DD (e.g., 2025-01-01).\n\n");
            
            printf("2. By Month and Year:\n");
            printf("   - View all transactions for a specific month.\n");
            printf("   - Useful for checking your monthly activity at a glance.\n");

            printf("Pressed enter to continue...\n");
            getchar();
        }

        // --- TOPIC 4: REPORTS & CALCULATOR ---
        else if (strcmp(choice, "4") == 0) {
            clear_screen();
            printf("[ REPORTS & TOOLS ]\n\n");
            printf("1. CALCULATOR:\n");
            printf("   - A built-in tool to help you do quick math.\n");
            printf("   - Supports standard order of operations.\n\n");

            printf("2. GENERATE REPORT:\n");
            printf("   - Creates a summary of your financial health for a specific month.\n");
            printf("   - Enter the Month (1-12) and Year (e.g., 2025) to generate.\n");
            printf("   - Exported into a HTML file for easy reading.\n");

            printf("Pressed enter to continue...\n");
            getchar();
        }

        else if (strcmp(choice, "5") == 0) {
            break; 
        }

        else {
            printf("Invalid selection. Please try again.\n");
            SLEEP_SECONDS(2);
        }
    }
}

void menu(sqlite3 *db, User *user)
{
    clear_screen();
    char choice[5];

    while (1)
    {
        clear_screen();
        // print_header();
        printf("MAIN MENU\n");
        printf("----------------\n");
        printf("1. Wallets\n");
        printf("2. Register Income\n");
        printf("3. Register Spending\n");
        printf("4. History\n");
        printf("5. Calculator\n");
        printf("6. Generate Report\n");
        printf("7. Help\n");
        printf("8. Exit\n");
        printf("----------------\n");
        printf("Select option (1-8): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL)
            continue;

        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "1") == 0)
        {

            handle_wallet_menu(db, user->id);
        }
        else if (strcmp(choice, "2") == 0)
        {

            handle_income_menu(db, user->id);
        }
        else if (strcmp(choice, "3") == 0)
        {

            handle_spending_menu(db, user->id);
        }
        else if (strcmp(choice, "4") == 0)
        {
            handle_history_menu(db, user->id);
        }
        else if (strcmp(choice, "5") == 0)
        {
            handle_calculator_menu();
        }
        else if (strcmp(choice, "6") == 0)
        {
            handle_report_menu(db, user);
        }
        else if (strcmp(choice, "7") == 0)
        {
            handle_help_menu();
        }
        else if (strcmp(choice, "8") == 0)
        {
            int yn_input = get_yes_or_no_input("Are you sure you want to exit?", 0);

            if (yn_input)
            {
                yn_input = get_yes_or_no_input("Delete login credentials/Log out?", 0);
                if (yn_input)
                {
                    logout();
                }
                printf("Goodbye!\n");
                break;
            }
        }
        else
        {
            fprintf(stderr, "Invalid selection.");
        }
    }
}