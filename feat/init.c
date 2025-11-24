#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../db/db.h"
#include "./hash.h"
#include "./user.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void init(char *folder, char *filename)
{
    sqlite3 *db;
    int err;
    int result = -1;

    char *path;
    char *sql;

#ifdef _WIN32
    result = _mkdir(folder);
#else
    result = mkdir(folder, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

    if (result == -1)
    {
        if (errno == EEXIST)
        {
            printf("Folder '%s' already exists.\n", folder);
        }
        else
        {
            printf("Error creating folder '%s': %s\n", folder, strerror(errno));
        }
    }
    else
    {
        printf("Folder '%s' created successfully.\n", folder);
    }

    // tambah 1 byte buat / dan tambah 1 lagi buat \0
    path = (char *)malloc((strlen(folder) + strlen(filename)) + 2);

    if (path == NULL)
    {
        fprintf(stderr, "Memory allocation failed for varible path!\n");
        exit(1);
    }

    sprintf(path, "%s/%s", folder, filename);

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

    err = migrate_up(db);

    if (err != SQLITE_OK)
    {
        fprintf(stderr, "Migrations Error! %s\n", sqlite3_errmsg(db));
    }

    // TODO: ASK About Login info
    char username[64];
    char password[64];
    char confirmation[64];
    int valid = 1;

username_input:
    printf("Please enter your desired username (maks. 64, only alphanumeric, no spaces): \n");

    fgets(username, 64, stdin);
    username[strcspn(username, "\n")] = 0;

    for (int i = 0; username[i] != '\0'; i++)
    {
        if (!isalnum(username[i]))
        {
            valid = 0;
            break;
        }
    }

    if (!valid)
    {
        fprintf(stderr, "Username contains illegal character! please try again!");
        memset(username, 0, sizeof(username));
        goto username_input;
    }

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

    if (count != 0)
    {
        fprintf(stderr, "Username already exist! please try again!");
        memset(username, 0, sizeof(username));
        goto username_input;
    }

    sqlite3_finalize(stmt);

password_input:
    printf("Please enter your password (maks. 64, only alphanumeric, no spaces): \n");

    fgets(password, 64, stdin);
    password[strcspn(password, "\n")] = 0;

    printf("Please enter your password again: \n");

    fgets(confirmation, 64, stdin);
    confirmation[strcspn(confirmation, "\n")] = 0;

    if (strcmp(password, confirmation) != 0)
    {
        fprintf(stderr, "Password not matching! please try again!");
        memset(password, 0, sizeof(password));
        memset(confirmation, 0, sizeof(confirmation));
        goto password_input;
    }

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
        fprintf(stderr, "Password contains illegal character! please try again!");
        memset(password, 0, sizeof(password));
        memset(confirmation, 0, sizeof(confirmation));
        goto password_input;
    }

    unsigned long long_hashed = hash((unsigned char *)password);

    // must be unsigned char for some reason
    char hashed_password[32];
    snprintf(hashed_password, sizeof(hashed_password), "%ld", long_hashed);

    insert_user(db, username, hashed_password);

    // why 5? karena ada \n sama \0
    char input_buffer[5];
    int yn_input = 1;
    int is_budgeting_active = 0;
    while (1)
    {
        printf("Do you wanna use the automatic budgeting system (YES/no)? (50%%, 30%% 20%%): \n");
        fgets(input_buffer, 5, stdin);

        if (input_buffer[0] == 'Y' || input_buffer[0] == 'y' || input_buffer[0] == '\n')
        {
            break;
        }

        yn_input = 0;
        break;
    }

    memset(input_buffer, 0, 5);

    if (yn_input)
    {
        // TODO: DO BUDGETING STUFF

        is_budgeting_active = 1;
    }
    else
    {
        yn_input = 1;
        while (1)
        {
            printf("Do you wanna setup the budgeting system manually (YES/no)? (Custom percantage for needs, wants, saving): \n");
            fgets(input_buffer, 5, stdin);

            if (input_buffer[0] == 'Y' || input_buffer[0] == 'y' || input_buffer[0] == '\n')
            {
                break;
            }

            yn_input = 0;
            break;
        }

        memset(input_buffer, 0, 5);

        if (yn_input)
        {
             // TODO: DO BUDGETING STUFF MANUALLY
            is_budgeting_active = 1;
        }
    }
    if(!is_budgeting_active){
        // TODO: MAKE A MAIN WALLET
    }
    
    double money;
    printf("Please input the amount of money you have right now: \n");
    scanf("%lf", &money);

    // TODO: income input

    // REMINDER: MUST BE DONE TOMMOROW AND READY TO USE TOO

    free(path);
    free(sql);
    path = NULL;
    sql = NULL;

    sqlite3_close(db);

    exit(0);
}