#include <stdio.h>
#include "../db/db.h"
#include <stdlib.h>
#include <string.h>
#include "category.h"
#include "budget_rule.h"

void free_category(Category *category){
    if (category != NULL) {

        free(category->name);
        free(category->type);
        free(category->budget_bucket);

        free(category);
    }
}


void free_categories(Category **categories){
    if (categories == NULL) {
        return;
    }

    int i = 0;
    while (categories[i] != NULL) {
        free_category(categories[i]);
        i++;
    }

    free(categories);
}



Category **get_all_categories(sqlite3 *db)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name, type, budget_bucket FROM Categories";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    Category **categories = NULL;
    int count = 0;

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:

            Category **temp = realloc(categories, sizeof(Category *) * (count + 1));

            if (temp == NULL)
            {
                fprintf(stderr, "Memory allocation failed for array\n");
                free_categories(categories);
                exit(1);
            }

            categories = temp;

            categories[count] = (Category *)malloc(sizeof(Category));

            if (categories[count] == NULL)
            {
                fprintf(stderr, "Memory allocation failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                exit(1);
            }

            categories[count]->id = sqlite3_column_int(stmt, 0);

            const unsigned char *raw_name_text = sqlite3_column_text(stmt, 1);

            if (raw_name_text)
            {
                categories[count]->name = strdup((char *)raw_name_text);
            }
            else
            {
                categories[count]->name = strdup(""); 
            }

            const unsigned char *raw_type_text = sqlite3_column_text(stmt, 2);

            if (raw_type_text)
            {
                categories[count]->type = strdup((char *)raw_type_text);
            }
            else
            {
                categories[count]->type = strdup("");
            }

            const unsigned char *raw_budget_bucket_text = sqlite3_column_text(stmt, 3);

            if (raw_budget_bucket_text)
            {
                categories[count]->budget_bucket = strdup((char *)raw_budget_bucket_text);
            }
            else
            {
                categories[count]->budget_bucket = strdup("");
            }

            count++;
            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);

    Category **temp = realloc(categories, sizeof(Category*) * (count + 1));
    if (temp) {
        categories = temp;
        categories[count] = NULL; 
    }

    return categories;
}
