#include "utils.h"
#include <ctype.h>
#ifdef _WIN32
    #include <conio.h>
    #include <direct.h>
    #define MKDIR(d) _mkdir(d)
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(d) mkdir(d, 0777)
#endif


#define CIPHER_KEY "]a~HGtsy$9xXe,neihado8w071023h hqw9*)&# !)HEA)HShdK%^BAI*WV*&^#!G X(WQU HHOQWH)" 

void clear_screen() {
    // A simple way to clear screen on both Windows and Linux/Mac
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


void print_centered(const char* text) {
    // Simple logic to center text (assuming standard 80 char width)
    int len = strlen(text);
    int padding = (80 - len) / 2;
    for(int i = 0; i < padding; i++) printf(" ");
    printf("%s\n", text);
}

void get_password(char *password, int max_len) {
    int i = 0;
    
    #ifdef _WIN32
        // Windows implementation using _getch()
        char ch;
        while (1) {
            ch = _getch();
            if (ch == 13) { // Enter key
                password[i] = '\0';
                printf("\n");
                break;
            } else if (ch == 8) { // Backspace
                if (i > 0) {
                    i--;
                    printf("\b \b"); // Erase character on screen
                }
            } else if (i < max_len - 1) {
                password[i] = ch;
                i++;
                printf("*"); // Mask with asterisk
            }
        }
    #else
        // Linux/Mac implementation using termios to disable ECHO
        struct termios oldt, newt;
        
        // Get current terminal attributes
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        
        // Disable local echo
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // Read the line
        if (fgets(password, max_len, stdin) != NULL) {
            // Remove newline character
            password[strcspn(password, "\n")] = 0;
        }

        // Restore original terminal attributes
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
}

int get_yes_or_no_input(char *question, int default_value) {
    char line[256]; // Buffer to hold the input line
    int input_int = -1;

    while (1) {
        // 1. Better prompt formatting
        // Capitalize the default option (Y/n) or (y/N)
        printf("%s (%s/%s): ", question, 
               (default_value == 1) ? "Y" : "y", 
               (default_value == 0) ? "N" : "n");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // Handle error or EOF (optional, usually return default)
            return (default_value != -1) ? default_value : 0;
        }

        if (line[0] == '\n') {
            if (default_value != -1) {
                return default_value;
            }
            // If no default, loop and ask again
            continue; 
        }

        // 4. Check the first character
        char choice = line[0];

        if (choice == 'y' || choice == 'Y') {
            return 1;
        } 
        else if (choice == 'n' || choice == 'N') {
            return 0;
        }
    }
}

void trim_newline(char *str) {
    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
        len--; // Update length
    }
    
    if (len > 0 && str[len - 1] == '\r') {
        str[len - 1] = '\0';
    }
}

int get_int_input(char *question) {
    char buffer[1024]; // Large buffer to hold user input
    char *endptr;
    long value;

    while (1) {
        printf("%s: ", question);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0; 
        }

        if (buffer[0] == '\n') {
            printf("Error: Input cannot be empty. Try again.\n");
            continue;
        }

        trim_newline(buffer);

        value = strtol(buffer, &endptr, 10);

        if (endptr == buffer || *endptr != '\0') {
            printf("Error: Invalid integer. Please enter a whole number.\n");
        } else {
            return (int)value;
        }
    }
}

double get_double_input(char *question) {
    char buffer[1024];
    char *endptr;
    double value;

    while (1) {
        printf("%s (use point for decimal): ", question);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0.0;
        }

        if (buffer[0] == '\n') {
            printf("Error: Input cannot be empty. Try again.\n");
            continue;
        }

        trim_newline(buffer);

        value = strtod(buffer, &endptr);

        if (endptr == buffer || *endptr != '\0') {
            printf("Error: Invalid number. Please enter a valid decimal.\n");
        } else {
            return value;
        }
    }
}
void clear_input_buffer(){
    char c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void print_line(int *widths, int columns) {
    printf("+");
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < widths[i] + 2; j++) {
            printf("-");
        }
        printf("+");
    }
    printf("\n");
}

int fileExists(const char *filename) {
    FILE *file = fopen(filename, "r"); // Attempt to open for reading
    if (file != NULL) {
        fclose(file); // Close the file if successfully opened
        return 1; // File exists
    }
    return 0; // File does not exist or cannot be opened
}

void ensure_data_dir() {
    // Try to create the directory. 
    // If it exists, this will fail/return -1, which we ignore safely.
    MKDIR("data");
}

char *xor_cipher_string(char *string){
    char *key = CIPHER_KEY;
    int keylen = strlen(key);

    for(int i = 0; i < strlen(string); i++){
        string[i] ^= key[i % keylen];
    }

    return string;
}