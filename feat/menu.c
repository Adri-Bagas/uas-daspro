#include "menu.h"
#include "auth.h"

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
            // Note: If you have a function that takes 'wallets' array, use that.
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

            if(filtered != NULL){
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
        printf("5. Calculate Debts\n");
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
        }
        else if (strcmp(choice, "6") == 0)
        {
        }
        else if (strcmp(choice, "7") == 0)
        {
            // Tampilkan help (Asumsi show_help() mencetak ke layar)

            // show_help(); // Uncomment jika show_help sudah diimplementasi
            printf("Feature HELP is coming soon...\n");
            printf("\nPress Enter to return...");
            getchar();
        }
        else if (strcmp(choice, "8") == 0)
        {
            break;
        }
        else
        {
            fprintf(stderr, "Invalid selection.");
        }
    }
}