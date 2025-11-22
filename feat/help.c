#include <stdio.h>
#include <string.h>

#define PROGRAM_NAME "program"
#define VERSION "0.0.1"

// ANSI Color Codes
#define BOLD  "\033[1m"
#define GREEN "\033[0;32m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

void print_help(const char *exec_name) {
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