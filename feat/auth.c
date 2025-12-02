#include "auth.h"
#include "hash.h"
#include "user.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

// Return user
User *login(sqlite3 *db) {
  char username[64];
  char password[64];
  int valid = 1;

login_username_input:
  printf("Please enter your username (maks. 64, only alphanumeric, no spaces): "
         "\n");
  fgets(username, 64, stdin);
  username[strcspn(username, "\n")] = 0;

  for (int i = 0; username[i] != '\0'; i++) {
    if (!isalnum(username[i])) {
      valid = 0;
      break;
    }
  }

  if (!valid) {
    fprintf(stderr, "Username contains illegal character! please try again!");
    memset(username, 0, sizeof(username));
    goto login_username_input;
  }

login_password_input:
  printf("Please enter your password (maks. 64, only alphanumeric, no spaces): "
         "\n");
  fgets(password, 64, stdin);
  password[strcspn(password, "\n")] = 0;

  valid = 1;

  for (int i = 0; password[i] != '\0'; i++) {
    if (!isalnum(password[i])) {
      valid = 0;
      break;
    }
  }

  if (!valid) {
    fprintf(stderr, "Password contains illegal character! please try again!");
    memset(password, 0, sizeof(password));
    goto login_password_input;
  }

  User *user = get_user_by_username(db, username);

  if (user == NULL) {
    exit(1);
  }

  unsigned long long_hashed = hash((unsigned char *)password);

  // must be unsigned char for some reason
  char hashed_password[32];
  snprintf(hashed_password, sizeof(hashed_password), "%ld", long_hashed);

  if (strcmp(hashed_password, user->password) == 0) {
    return user;
  }

  free_user(user);
  exit(1);
}
