#ifndef CATEGORY_H
#define CATEGORY_H

typedef struct
{
    int id;
    char *name;
    char *type;
    char *budget_bucket;
} Category;

Category **get_all_categories(sqlite3 *db);
void free_category(Category *category);
void free_categories(Category **categories);

#endif