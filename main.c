#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "./db/db.h"
#include "./feat/init.h"
#include "./feat/help.h"
#include "./feat/menu.h"
#include "./feat/auth.h"

#define DATA_FOLDER "./data"
#define FILENAME "data.db"

int main(int argc, char *argv[])
{
    // Untuk menyimpan data user
    User *user = NULL;

    // check jika tidak ada command
    if (argc < 2)
    {
        print_help(argv[0]);
        return 0;
    }

    char *command = argv[1];

    // check jika command init
    if (strcmp(argv[1], "init") == 0 || strcmp(argv[1], "-i") == 0)
    {
        init(DATA_FOLDER, FILENAME);
        return 0;
    }

    // check jika command help
    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        print_help(argv[0]);
        return 0;
    }

    // check jika command menu
    if(strcmp(argv[1], "menu") == 0 || strcmp(argv[1], "-m") == 0){

        // inisiasi database pertama, yang akan di gunakan untuk segala operasi dalam menu
        sqlite3 *db;

        // membuata atau membaca db
        openOrCreateDB(DATA_FOLDER "/" FILENAME, &db);

        // login user
        user = login(db);

        if (user == NULL)
        {
            return 1;
        }

        // jika berhasil login buka menu
        menu(db, user);

        // ketika menu selesai, matikan databasenya
        close_db(db);
        return 0;
    }

    // command tidak diketahui
    fprintf(stderr, "Unknown command: '%s'\n", command);
    fprintf(stderr, "Type '%s help' to see available commands.\n", argv[0]);
    return 1;
}