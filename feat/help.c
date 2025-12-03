#include <stdio.h>
#include <string.h>
#include "utils.h"

#define PROGRAM_NAME "program"
#define VERSION "0.0.1"

// ANSI Color Codes
#define BOLD  "\033[1m"
#define GREEN "\033[0;32m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

void show_header()
{
    clear_screen();

    printf("\n\n");
    // printf(COLOR_GREEN);
    printf("$$$   FINANCE MANAGER CLI   $$$\n");
    // printf(COLOR_RESET);
    printf("========================================");
    printf("\n");

    printf("%sWelcome to your path to Financial Freedom!%s\n\n", BOLD, RESET);

    printf("This application helps you organize your money efficiently.\n");
    printf("Here is what you can do:\n\n");

    printf("%s1. Smart Wallets:%s\n", GREEN, RESET);
    printf("  Manage multiple accounts (Main, Emergency, Savings) in one place.\n\n");

    printf("%s2. Track Everything:%s\n", GREEN, RESET);
    printf("  Record Income and Expenses with custom categories.\n\n");

    printf("%s3. Goals & Debts:%s\n", GREEN, RESET);
    printf("  Set Savings Goals and track Debt repayments seamlessly.\n\n");

    printf("%s4. Budgeting:%s\n", GREEN, RESET);
    printf("  Apply the 50/30/20 rule to control your spending habits.\n\n");
}

void print_help(const char *exec_name) {

    show_header();

    // BOLD usage
    printf("\n" BOLD PROGRAM_NAME RESET " - Version " VERSION "\n\n");
    
    printf(BOLD "USAGE:" RESET "\n");
    printf("  %s %s<command>%s [options]\n\n", exec_name, CYAN, RESET);

    printf(BOLD "AVAILABLE COMMANDS:" RESET "\n");
    
    // We use GREEN for the command name to make it pop
    printf("  " GREEN "%-12s" RESET " %s\n", "init | -i", "Initialize a new project configuration");
    printf("  " GREEN "%-12s" RESET " %s\n", "help | -h", "Show this help message");
    
    printf("\n");
}

