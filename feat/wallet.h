#ifndef WALLET_H
#define WALLET_H

typedef struct 
{
    int id;
    int user_id;
    char name[50];
    double balance;
    int allocation;
} Wallet;

void free_wallet(Wallet *wallet);
void free_wallets(Wallet **wallets);
int create_wallet_first_time(sqlite3 *db, int userId);
int create_wallet(sqlite3 *db, char *walletName, int userId, int allocation, int is_main);
long long create_wallet_return_id(sqlite3 *db, char *walletName, int userId, int allocation, int is_main);
Wallet **get_all_wallets_by_user_id(sqlite3 *db, int userId);
Wallet **get_all_wallets_by_user_id_that_are_main(sqlite3 *db, int userId);

#endif