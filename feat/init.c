#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "../db/db.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void init(char *folder, char *filename)
{

    sqlite3 *db;
    char *zErrMsg = 0;
    int err;
    int result = -1;

    char *path;

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

    sqlite3_close(db);

    exit(0);
}