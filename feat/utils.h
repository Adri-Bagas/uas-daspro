#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void clear_screen();
void print_centered(const char* text);
void get_password(char *password, int max_len);
int get_yes_or_no_input(char *question, int default_value);
int get_int_input(char *question);
double get_double_input(char *question);
void clear_input_buffer();

#endif