#include "menu.h"
#include "auth.h"

void handle_wallet_menu(sqlite3 *db, int user_id)
{
    clear_screen();
    Wallet **wallets = get_all_wallets_by_user_id(db, user_id);

    int is_restart = 0;

    int count = 0;

    char choice[5];

    while (1)
    {
        if (wallets != NULL)
        {
            show_wallet(wallets);
        }

        // printf("\n");

        printf("WALLETS MENU\n");
        printf("----------------\n");
        printf("1. Add Wallet\n");
        printf("2. Delete Wallet\n");
        printf("3. Transfer Funds\n");
        printf("4. Return\n");
        printf("----------------\n");
        printf("Select option (1-4): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL)
            continue;

        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "1") == 0)
        {
            clear_screen();
            int wallet_id;
            printf("Enter wallet name: ");

            char wallet_name[64];
            fgets(wallet_name, 64, stdin);
            wallet_name[strcspn(wallet_name, "\n")] = 0;

            wallet_id = create_wallet(db, wallet_name, user_id, 0, 0);

            if (wallet_id == -1)
            {
                printf("Failed to create wallet\n");
            }

            if (wallet_id != -1)
            {
                printf("Wallet created with ID: %d\n", wallet_id);
            }
        }
        else if (strcmp(choice, "2") == 0)
        {

            clear_screen();

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
                    is_restart = 1;
                    break;
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

                int err = 1;
                err = delete_wallet(db, user_id, selected_wallet_id, selected_wallet_name);

                if (err == 0)
                {
                    printf("Wallet deleted successfully\n");
                }
                else
                {
                    printf("Failed to delete wallet\n");
                    fprintf(stderr, "Failed to delete wallet: %s\n", sqlite3_errmsg(db));
                }

                free_wallets(wallets);
                is_restart = 1;
                break;
            }
        }
        else if (strcmp(choice, "3") == 0)
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
                    free_wallets(wallets);
                    is_restart = 1;
                    break;
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
                    is_restart = 1;
                    break;
                }

                if (wallet_input > 0 && wallet_input <= i)
                {
                    from_wallet = wallets[wallet_input - 1];
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
                free_wallets(wallets);
                is_restart = 1;
                    break;
            }

            if (amount < 0)
            {
                printf("Invalid amount!\n");
                goto amount_input;
            }

            // transfer_funds(db, user_id, from_wallet, to_wallet, amount);
            is_restart = 1;
            break;
        }
        else if (strcmp(choice, "4") == 0)
        {
            break;
        }
    }

    free_wallets(wallets);

    if (is_restart)
    {
        handle_wallet_menu(db, user_id);
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

void menu(sqlite3 *db, User *user)
{
    char choice[5];

    while (1)
    {
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
            // Tampilkan help (Asumsi show_help() mencetak ke layar)

            // show_help(); // Uncomment jika show_help sudah diimplementasi
            printf("Feature HELP is coming soon...\n");
            printf("\nPress Enter to return...");
            getchar();
        }
        else if (strcmp(choice, "5") == 0)
        {
        }
        else if (strcmp(choice, "6") == 0)
        {
        }
        else if (strcmp(choice, "7") == 0)
        {
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