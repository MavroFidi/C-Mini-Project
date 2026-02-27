#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "users.h"
#include "database.h"

User *create_users(int n) {
    User *users = malloc(sizeof(User) * n);
    if (!users) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        printf("Enter name for user %d: ", i + 1);
        if (!fgets(users[i].name, sizeof(users[i].name), stdin)) {
            users[i].name[0] = '\0';
        } else {
            char *p = strchr(users[i].name, '\n');
            if (p) *p = '\0';
        }

        int code = rand() % 900000 + 100000;
        sprintf(users[i].passcode, "%06d", code);
    }

    return users;
}

void print_users(int n, User *users) {
    printf("\nAssigned users and passcodes:\n");
    for (int i = 0; i < n; i++) {
        printf("%d: Name=\"%s\", Passcode=\"%s\"\n",
               i + 1, users[i].name, users[i].passcode);
    }
}

void initialize_users(void) {
    int n;
    printf("Enter number of users: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        while (getchar() != '\n');
        printf("Invalid number.\n");
        return;
    }
    while (getchar() != '\n');

    User *users = create_users(n);
    if (!users) return;

    /* Save each user to the database */
    for (int i = 0; i < n; i++) {
        save_user(&users[i]);
    }

    print_users(n, users);
    free_users(users);
}

void free_users(User *users) {
    free(users);
}

int login_user(User *out_user) {
    char name[50];
    char passcode[16];

    printf("Enter name: ");
    if (!fgets(name, sizeof(name), stdin)) return 0;
    char *p = strchr(name, '\n');
    if (p) *p = '\0';

    printf("Enter passcode: ");
    if (!fgets(passcode, sizeof(passcode), stdin)) return 0;
    p = strchr(passcode, '\n');
    if (p) *p = '\0';

    return find_user(name, passcode, out_user);
}
