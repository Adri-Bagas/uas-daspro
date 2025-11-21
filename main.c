#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "./db/db.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

int main(int argc, char *argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int err;
    int result = -1;

    const char *folder = "./data";

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

    err = openOrCreateDB("./data/test_1.db", &db);

    if (err)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    if (argc > 1)
    {
        if (strcmp(argv[1], "init") == 0)
        {
            err = migrate_up(db);

            if(err != SQLITE_OK){
                fprintf(stderr, "Migrations Error! %s\n", sqlite3_errmsg(db));
            }
        }
    }

    sqlite3_close(db);

    return 0;
}