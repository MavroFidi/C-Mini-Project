#ifndef USERS_H
#define USERS_H

typedef struct {
    char name[50];
    char passcode[62];
} User;

void get_passcode_input(char *out_passcode);
User *create_users(int n);
void print_users(int n, User *users);
void initialize_users(void);
void free_users(User *users);
int login_user(User *out_user);

#endif
