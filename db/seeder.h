#ifndef SEEDER_H
#define SEEDER_H

#include "../sqlite/sqlite3.h"

/**
 * Inserts default categories for a newly registered user.
 * Includes standard categories for Income, Needs, Wants, and Savings.
 * * @param db Pointer to the SQLite database.
 * @param user_id The ID of the user to seed data for.
 */
int seed_default_categories(sqlite3 *db);

#endif