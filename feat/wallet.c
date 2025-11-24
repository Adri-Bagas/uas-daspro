#include <stdio.h>

struct wallet
{
    int id;
    int user_id;
    char name[50];
    double balance;
    int allocation;
};

int createWallet(userId)
{
    char walletName[50];
    double balance;
    int allocation = 100;
    printf("Masukkan nama dompet : ");
    scanf("%s", walletName);

    // Cek user punya dompet atau belum
    if (getUserWallets(userId))
    {
        // Reset allocation semua wallet
    }

    printf("Masukkan saldo awal dompet: ");
    scanf("%lf", &balance);
    if (createNewWalletInDB(userId, walletName, allocation, balance) == 0)
    {
        printf("Dompet '%s' berhasil dibuat untuk user ID %d.\n", walletName, userId);
    }
    else
    {
        printf("Gagal membuat dompet baru.\n");
        return -1;
    }

    return 0;
}