#include "auth.h"
#include "hash.h"
#include "user.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"

// Return user
User *login(sqlite3 *db) {
  char username[64];
  char password[64];
  int valid = 1;

  int err = 0;

  // pastikan dir data ada
  ensure_data_dir(); 

  // kalau ada file seession, baca isinya terus login pakai data yang sudah di decrypt
  if(fileExists("data/session.txt")){
    FILE *f = fopen("data/session.txt", "r");

    char buffer[128];

    fgets(buffer, 128, f);

    buffer[strcspn(buffer, "\n")] = 0;

    char *username = xor_cipher_string(buffer);

    User *user = get_user_by_username(db, username);

    if (user == NULL)
    {
      printf("User Info Not Found! %s\n", username);
    }

    if(user != NULL){
      printf("You are already logged in as %s\n", user->username);
      fclose(f);
      return user;
    }
  }

  // input username
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

  // input password
login_password_input:
  printf("Please enter your password (maks. 64, only alphanumeric, no spaces): "
         "\n");

  get_password(password, 64);

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

  //try hasihing password nya
  unsigned long long_hashed = hash((unsigned char *)password);

  // must be unsigned char for some reason
  char hashed_password[32];
  snprintf(hashed_password, sizeof(hashed_password), "%ld", long_hashed);

  // kalau sama password nya bener
  if (strcmp(hashed_password, user->password) == 0) {
    int yn_input = get_yes_or_no_input("Save login session?", 0);

    if(yn_input){
      FILE *f = fopen("data/session.txt", "w");

      fprintf(f, "%s", xor_cipher_string(username));

      fclose(f);
    }

    return user;
  }

  // kalo nggak langsung keluar
  free_user(user);
  exit(1);
}

// log out cuma hapus session
void logout(){
  if(fileExists("data/session.txt")){
    remove("data/session.txt");
  }
}