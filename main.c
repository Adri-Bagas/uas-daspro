#include <stdio.h>
#include <string.h>
#include "./db/db.h"
#include "./feat/init.h"
#include "./feat/help.h"
#include "./feat/menu.h"
#include "./feat/auth.h"

#define DATA_FOLDER "./data"
#define FILENAME "data.db"

int main(int argc, char *argv[])
{
    User *user = NULL;

    if (argc < 2)
    {
        print_help(argv[0]);
        return 0;
    }

    char *command = argv[1];

    if (strcmp(argv[1], "init") == 0 || strcmp(argv[1], "-i") == 0)
    {
        init(DATA_FOLDER, FILENAME);
        return 0;
    }

    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        print_help(argv[0]);
        return 0;
    }

    if(strcmp(argv[1], "menu") == 0 || strcmp(argv[1], "-m") == 0){

        sqlite3 *db;

        openOrCreateDB(DATA_FOLDER "/" FILENAME, &db);

        user = login(db);

        if (user == NULL)
        {
            return 1;
        }

        menu(db, user);

        close_db(db);
        return 0;
    }

    fprintf(stderr, "Unknown command: '%s'\n", command);
    fprintf(stderr, "Type '%s help' to see available commands.\n", argv[0]);
    return 1;
}