#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db/db.h"
#include "user.h"

// REMEMBER THIS ONE FOR IF ONLY YOU USE MALLOC - bagas
void free_user(User *data)
{
    if (data == NULL)
    {
        return;
    }

    free(data->password);
    free(data->created_at);    
    free(data);
}

User* get_user_by_username(sqlite3 *db, const char *username)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, password_hash FROM Users WHERE username = ?";
    User *user = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:
            /*
                COL list
                1. id
                2. username,
                3. password_hashed
                4. created_at
            */

            user = (User *)malloc(sizeof(User));

            if (user == NULL)
            {
                fprintf(stderr, "Failed to prepare memory for user");
                sqlite3_finalize(stmt);
                exit(1);
            }

            user->id = sqlite3_column_int(stmt, 0);

            const char *username_text = (const char *)sqlite3_column_text(stmt, 1);
            const char *password_text = (const char *)sqlite3_column_text(stmt, 2);

            if (!username_text)
            {
                fprintf(stderr, "Failed to extract data from col!");
                exit(1);
            }

            if (!password_text)
            {
                fprintf(stderr, "Failed to extract data from col!");
                exit(1);
            }

            strcpy(user->username, username_text);

            user->password = strdup(password_text);

            user->created_at = NULL;

            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);

    if (user == NULL)
    {
        fprintf(stderr, "User not found!\n");
    }

    return user;
}

long long insert_user(sqlite3 *db, const char *username, const unsigned char *hash)
{
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Users (username, password_hash) VALUES (?, ?)";
    long long new_id = 0;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_DONE)
    {
        new_id = sqlite3_last_insert_rowid(db);
        printf("User '%s' inserted successfully.\n", username);
    }
    else
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return new_id;
}