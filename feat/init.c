#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../db/db.h"
#include "./hash.h"
#include "./user.h"
#include "./wallet.h"
#include "./budget_rule.h"
#include "income.h"
#include "utils.h"

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define SLEEP_SECONDS(seconds) Sleep(seconds * 1000)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define SLEEP_SECONDS(seconds) sleep(seconds)
#endif

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_BOLD "\033[1m"

void show_onboarding()
{
    clear_screen();

    printf("\n\n");
    // printf(COLOR_GREEN);
    printf("$$$   FINANCE MANAGER CLI   $$$\n");
    // printf(COLOR_RESET);
    printf("========================================");
    printf("\n");

    printf("%sWelcome to your path to Financial Freedom!%s\n\n", COLOR_BOLD, COLOR_RESET);

    printf("This application helps you organize your money efficiently.\n");
    printf("Here is what you can do:\n\n");

    printf("%s1. Wallets:%s\n", COLOR_GREEN, COLOR_RESET);
    printf("  Manage multiple wallets (Needs, Wants, Savings).\n\n");

    printf("%s2. Track Everything:%s\n", COLOR_GREEN, COLOR_RESET);
    printf("  Record Income and Expenses with custom categories.\n\n");

    printf("%s3. Savings & Debts:%s\n", COLOR_GREEN, COLOR_RESET);
    printf("  Calculate Savings and Debt interest.\n\n");

    printf("%s4. Budgeting:%s\n", COLOR_GREEN, COLOR_RESET);
    printf("  Apply the 50/30/20 rule to control your spending habits.\n\n");

    printf("----------------------------------------\n");
    printf("Let's get you set up with a user profile.\n");
    printf("----------------------------------------\n" COLOR_RESET);
}


// menerima folder dan filename
void init(char *folder, char *filename)
{
    // inisiasi variable
    sqlite3 *db;
    int err;
    int result = -1;

    char *path;
    char *sql;

    // tambah 1 byte buat / dan tambah 1 lagi buat \0
    path = (char *)malloc((strlen(folder) + strlen(filename)) + 2);

    if (path == NULL)
    {
        fprintf(stderr, "Memory allocation failed for varible path!\n");
        exit(1);
    }

    sprintf(path, "%s/%s", folder, filename);

    // coba buat / baca db, jika berhasil lanjutkan, gagal close program
    err = openOrCreateDB(path, &db);

    if (err)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        free(path);
        exit(1);
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    // jalan kan migrasi
    err = migrate_up(db);

    if (err != SQLITE_OK)
    {
        fprintf(stderr, "Migrations Error! %s\n", sqlite3_errmsg(db));
        close_db(db);
        exit(1);
    }

    // jalan kan seeder
    err = seed_default_categories(db);

    if (err)
    {
        fprintf(stderr, "Migrations Error! %s\n", sqlite3_errmsg(db));
        close_db(db);
        exit(1);
    }
    
    // tinggu 5 detik agar user dapat diberikan ruang waktu
    SLEEP_SECONDS(3);
    // TODO: ASK About Login info

    // menampilkan onboarding screen
    show_onboarding();

    // USE 64 Untuk username and password max length
    char username[64];
    char password[64];
    char confirmation[64];
    int valid = 1;
// Input Username
username_input:
    printf("Please enter your desired username (maks. 64, only alphanumeric, no spaces): \n");

    fgets(username, 64, stdin);
    username[strcspn(username, "\n")] = 0;

    for (int i = 0; username[i] != '\0'; i++)
    {
        // untuk cek apakah alphanumeric
        if (!isalnum(username[i]))
        {
            valid = 0;
            break;
        }
    }

    if (!valid)
    {
        fprintf(stderr, "Username contains illegal character! please try again!\n");
        strcpy(username, "");
        goto username_input;
    }

    // cek apakah username sudah ada
    sql = strdup("SELECT COUNT(*) FROM users WHERE username = ?");

    if (sql == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for username!");
        exit(1);
    }

    sqlite3_stmt *stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    else
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    // jika sudah ada munculkan error
    if (count != 0)
    {
        fprintf(stderr, "Username already exist! please try again!");
        strcpy(username, "");
        // goto dipakai untuk pengganti while loop, lebih mudah dibaca
        goto username_input;
    }

    sqlite3_finalize(stmt);

// input password yang diignkan
password_input:
    printf("Please enter your password (maks. 64, only alphanumeric, no spaces): \n");

    // fgets(password, 64, stdin);
    // password[strcspn(password, "\n")] = 0;

    get_password(password, 64);

    printf("Please enter your password again: \n");

    // fgets(confirmation, 64, stdin);
    // confirmation[strcspn(confirmation, "\n")] = 0;

    get_password(confirmation, 64);

    valid = 1;

    for (int i = 0; password[i] != '\0'; i++)
    {
        if (!isalnum(password[i]))
        {
            valid = 0;
            break;
        }
    }

    if (!valid)
    {
        fprintf(stderr, "Password contains illegal character! please try again!\n");
        strcpy(password, "");
        strcpy(confirmation, "");
        goto password_input;
    }

    // kurang lebih sama seperti tadi, hanya saja kali ini kita cek apakah password untuk konfirmasi sudah sama atau tidak
    if (strcmp(password, confirmation) != 0)
    {
        fprintf(stderr, "Password not matching! please try again!\n");
        strcpy(password, "");
        strcpy(confirmation, "");
        goto password_input;
    }

    unsigned long long_hashed = hash((unsigned char *)password);

    // must be unsigned char for some reason
    char hashed_password[32];
    snprintf(hashed_password, sizeof(hashed_password), "%ld", long_hashed);

    long long userId = 0;
    userId = insert_user(db, username, hashed_password);

    int yn_input = 1;
    int is_budgeting_active = 0;

    // tanyakan mau pakai budgeting automatis?
    yn_input = get_yes_or_no_input("Do you wanna use the automatic budgeting system (50% needs, 30% wants, 20% savings) ?:", 1);

    // jika ya buatkan langsung
    if (yn_input)
    {
        // TODO: DO BUDGETING STUFF
        // CREATE BUDGET RULE AUTOMATIC

        yn_input = 1;

        double target_income = 0;

        yn_input = get_yes_or_no_input("Do you want to set a monthly target income ?:", 1);

        if (yn_input)
        {
            target_income = get_double_input("Please enter your monthly target income: ");
        }

        err = create_budget_rules(db, userId, 50, 30, 20, target_income);

        if (err)
        {
            fprintf(stderr, "Failed to create budget rules: %s\n", sqlite3_errmsg(db));
            close_db(db);
            exit(1);
        }

        is_budgeting_active = 1;
    }
    // jika tidak maka tanyakan mau buat manual? atau tidak pakai sama sekali
    else
    {
        yn_input = get_yes_or_no_input("Do you want to setup the budgeting system manually ?:", 1);

        if (yn_input)
        {
            // TODO: DO BUDGETING STUFF MANUALLY
            // CREATE BUDGET RULES

            int needs = 0, wants = 0, savings = 0;
            int base_percentage = 100;
            char input_buffer[5];

            while (1)
            {
                base_percentage = 100;
                printf("Please enter percentage for needs: \n");
                fgets(input_buffer, 5, stdin);
                needs = atoi(input_buffer);

                if (needs > base_percentage)
                {
                    printf("Needs percentage must be less than %d%. Please try again.\n", base_percentage);
                    memset(input_buffer, 0, 5);
                    continue;
                }

                base_percentage -= needs;

                printf("Please enter percentage for wants: \n");
                fgets(input_buffer, 5, stdin);
                wants = atoi(input_buffer);

                if (wants > base_percentage)
                {
                    printf("Wants percentage must be less than %d%. Please try again.\n", base_percentage);
                    memset(input_buffer, 0, 5);
                    continue;
                }

                base_percentage -= wants;

                printf("Please enter percentage for savings: \n");
                fgets(input_buffer, 5, stdin);
                savings = atoi(input_buffer);

                if (savings > base_percentage)
                {
                    printf("Savings percentage must be less than %d%. Please try again.\n", base_percentage);
                    memset(input_buffer, 0, 5);
                    continue;
                }

                base_percentage -= savings;

                if (needs + wants + savings == 100)
                {
                    break;
                }
                else
                {
                    printf("Total percentage must be 100%. Please try again.\n");
                    memset(input_buffer, 0, 5);
                }
            }

            yn_input = 1;

            double target_income = 0;

            yn_input = get_yes_or_no_input("Do you want to set a monthly target income ?:", 1);

            memset(input_buffer, 0, 5);

            if (yn_input)
            {
                target_income = get_double_input("Please enter your monthly target income: ");
            }

            err = create_budget_rules(db, userId, needs, wants, savings, target_income);

            if (err)
            {
                fprintf(stderr, "Failed to create budget rules: %s\n", sqlite3_errmsg(db));
                close_db(db);
                exit(1);
            }

            is_budgeting_active = 1;
        }
    }

    long long wallet_id = 0;

    // yang tadi jika budgeting tidak aktif maka buat main wallet
    if (!is_budgeting_active)
    {
        // TODO: MAKE A MAIN WALLET
        printf("Budgeting is not active, creating a main wallet for you.\n");

        wallet_id = create_wallet(db, "Main", userId, 0, 1);

        if (wallet_id <= 0)
        {
            fprintf(stderr, "Failed to prepare create Main wallet: %s\n", sqlite3_errmsg(db));
            close_db(db);
            exit(1);
        }

        printf("Main wallet created.\n");
    }
    else
    // jika budgeting aktif buat wallet sesuai aturan budget nya
    {
        printf("Budgeting is active.\n");

        err = create_wallet_first_time(db, userId);

        if (err != 0)
        {
            fprintf(stderr, "Failed to prepare create wallets: %s\n", sqlite3_errmsg(db));
            close_db(db);
            exit(1);
        }
    }

    yn_input = 1;

    // tanyakan mau add money
    yn_input = get_yes_or_no_input("Do you want to add some money to the balance ?", 1);

    // kalau mau catat sebagai income
    if (yn_input)
    {
        create_income(db, userId, (int)wallet_id, "Main");
    }

    free(path);
    free(sql);
    path = NULL;
    sql = NULL;

    close_db(db);

    // selesai
    printf("Finished setting up your user profile.\n");
    printf("You can now start organizing your money.\n");

    printf("run the menu command to access the menu, make sure to login with the account that has been just created!");

    exit(0);
}