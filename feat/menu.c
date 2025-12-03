#include "menu.h"
#include "auth.h"


void handle_wallet_menu(sqlite3 *db, int user_id) {
    Wallet **wallets = get_all_wallets_by_user_id(db, user_id);

    int count = 0;

    if (wallets != NULL)
    {
        show_wallet(wallets); 
    }

    printf("\nPress Enter to return...");
    getchar();

    free_wallets(wallets);
}

void handle_income_menu(sqlite3 *db, int user_id) {
    create_income(db, user_id, 0, NULL);
}

void handle_spending_menu(sqlite3 *db, int user_id) {
    create_spending(db, user_id, 0, NULL, 0);
}

void menu(sqlite3 *db, User* user) {
    char choice[10];

    while (1) {
        // print_header();
        printf("MAIN MENU\n");
        printf("----------------\n");
        printf("1. Wallets\n");
        printf("2. Register Income\n");
        printf("3. Register Spending\n");
        printf("4. Generate Report\n");
        printf("7. Help\n");
        printf("8. Exit\n");
        printf("----------------\n");
        printf("Select option (1-5): ");

        if (fgets(choice, sizeof(choice), stdin) == NULL) continue;

        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "1") == 0) {
            handle_wallet_menu(db, user->id);
        } else if (strcmp(choice, "2") == 0) {
            handle_income_menu(db, user->id);
        } else if (strcmp(choice, "3") == 0) {
           
        } else if (strcmp(choice, "4") == 0) {
            // Tampilkan help (Asumsi show_help() mencetak ke layar)

            // show_help(); // Uncomment jika show_help sudah diimplementasi
            printf("Feature HELP is coming soon...\n"); 
            printf("\nPress Enter to return...");
            getchar();
        } else if (strcmp(choice, "5") == 0) {
            break;
        } else {
            fprintf(stderr,"Invalid selection.");
        }
    }
}