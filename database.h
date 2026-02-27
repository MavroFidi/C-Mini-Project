#ifndef DATABASE_H
#define DATABASE_H

#include "users.h"

int  init_database(void);
void close_database(void);
void save_user(User *user);
int  find_user(const char *name, const char *passcode, User *out_user);
void notes_menu(const char *username);
void reset_users(void);

#endif
