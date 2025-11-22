#include <stdio.h>
#include <string.h>
#include "./db/db.h"
#include "./feat/init.h"
#include "./feat/help.h"

#define DATA_FOLDER "./data"
#define FILENAME "data.db"

int main(int argc, char *argv[])
{
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

    fprintf(stderr, "Unknown command: '%s'\n", command);
    fprintf(stderr, "Type '%s help' to see available commands.\n", argv[0]);
    return 1;
}