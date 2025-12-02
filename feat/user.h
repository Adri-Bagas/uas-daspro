#ifndef USER_H
#define USER_H
#include "../sqlite/sqlite3.h"

typedef struct
{
    int id;
    // although on the database the data is not limited, from the c program is limited to 64 char
    char username[64];
    char *password;
    char *created_at;
} User;

User* get_user_by_username(sqlite3 *db, const char *username);
long long insert_user(sqlite3 *db, const char *username, const unsigned char *hash);
void free_user(User *data);

#endif