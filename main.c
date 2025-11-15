#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "./sqlite/sqlite3.h"

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// typedef struct Company {
//     int id;
//     char* name;
//     int age;
//     char* address;
//     float salary;
// } Company;

// static int callback(void *Company, int argc, char **argv, char **azColName)
// {
//     // int i;
//     // for (i = 0; i < argc; i++)
//     // {
//     //     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     // }
//     // printf("\n");
//     struct Company *tmp = (struct Company *)Company;

//     char missing[] = "MISSING";

//     tmp->id = argv[0] ? atoi(argv[0]) : -1;

//     char* tmpName =  argv[1] ? argv[1] : (char *)"NULL";
//     tmp->name = (char *)malloc(strlen(tmpName) + 1);

//     if(tmp->name) {
//         strcpy(tmp->name, tmpName);
//     }

//     tmp->age = argv[2] ? atoi(argv[2]) : -1;

//     char* tmpAddress =  argv[3] ? argv[3] : (char *)"NULL";
//     tmp->address = (char *)malloc(strlen(tmpAddress) + 1);

//     if(tmp->address) {
//         strcpy(tmp->address, tmpAddress);
//     }

//     tmp->salary = argv[4] ? atof(argv[4]) : -1.0;

//     return 0;
// }

int main(int argc, char *argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
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

    rc = sqlite3_open("./data/example.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    // /* Create SQL statement */
    // sql = "CREATE TABLE COMPANY("
    //       "ID INT PRIMARY KEY     NOT NULL,"
    //       "NAME           TEXT    NOT NULL,"
    //       "AGE            INT     NOT NULL,"
    //       "ADDRESS        CHAR(50),"
    //       "SALARY         REAL );";

    // /* Execute SQL statement */
    // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    // if (rc != SQLITE_OK)
    // {
    //     fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    // }
    // else
    // {
    //     fprintf(stdout, "Table created successfully\n");
    // }

    /* Create SQL statement */
    // sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
    //       "VALUES (1, 'Paul', 32, 'California', 20000.00 ); "
    //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
    //       "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "
    //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
    //       "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"
    //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
    //       "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

    // /* Execute SQL statement */
    // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    // if (rc != SQLITE_OK)
    // {
    //     fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    // }
    // else
    // {
    //     fprintf(stdout, "Records created successfully\n");
    // }

    // Company *companies = (Company *) malloc(sizeof (Company));

    // sql = "SELECT * FROM company LIMIT 1;";

    // rc = sqlite3_exec(db, sql, callback, companies, &zErrMsg);

    // if (rc != SQLITE_OK)
    // {
    //     fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    // }
    // else
    // {
    //     fprintf(stdout, "Operation done successfully\n");
    // }

    // printf("Hi my name is: %s with address at %s\n", companies->name, companies->address);

    sqlite3_close(db);

    return 0;
}