#include "utils.h"
#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

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

int get_yes_or_no_input(char *question, int default_value){
    char input;
    int input_int = default_value;
    while(1){
        printf("%s (%s/%s): ", question, default_value ? "Y" : "y", !default_value ? "N" : "n");
        scanf("%c", &input);
        if (input == 'y' || input == 'Y' || input == 'n' || input == 'N') {
            input_int = (input == 'y' || input == 'Y') ? 1 : 0;
            break;
        };
        if(default_value != -1){
            input_int = default_value;
            break;
        }
    }
   
    clear_input_buffer();
    return input_int;
}

int get_int_input(char *question){
    int input;
    printf("%s: ", question);
    scanf("%d", &input);
    clear_input_buffer();
    return input;
}

double get_double_input(char *question){
    double input;
    printf("%s (use point for decimal): ", question);
    scanf("%lf", &input);
    clear_input_buffer();
    return input;
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