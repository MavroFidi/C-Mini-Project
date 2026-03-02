#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "users.h"
#include "database.h"
#include "encryption.h"

void get_passcode_input(char *out_passcode) {
    char raw[52];

    while (1) {
        printf("  Enter password (10-50 characters): ");
        if (!fgets(raw, sizeof(raw), stdin)) {
            raw[0] = '\0';
            continue;
        }

        char *nl = strchr(raw, '\n');
        if (!nl) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("  Too long. Maximum is 50 characters.\n");
            continue;
        }
        *nl = '\0';

        size_t len = strlen(raw);
        if (len < 10) {
            printf("  Too short. Minimum is 10 characters.\n");
            continue;
        }

        /* Generate a random salt */
        unsigned int salt = (unsigned int)rand();

        /* Scramble with the salt */
        char scrambled[51];
        substitution_cipher(raw, scrambled, salt);

        /* Store as "SALT:SCRAMBLED" */
        sprintf(out_passcode, "%u:%s", salt, scrambled);

        printf("  Scrambled passcode: %s\n", scrambled);
        break;
    }
}

/* ── create_users ────────────────────────────────────────────────────────── */
User *create_users(int n) {
    User *users = malloc(sizeof(User) * n);
    if (!users) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        printf("User %d:\n", i + 1);

        printf("  Enter name: ");
        if (!fgets(users[i].name, sizeof(users[i].name), stdin)) {
            users[i].name[0] = '\0';
        } else {
            char *p = strchr(users[i].name, '\n');
            if (p) *p = '\0';
        }

        get_passcode_input(users[i].passcode);
    }

    return users;
}

/* ── print_users ─────────────────────────────────────────────────────────── */
void print_users(int n, User *users) {
    printf("\nAssigned users and scrambled passcodes:\n");
    for (int i = 0; i < n; i++) {
        /* Print only the scrambled part (after the colon) for display */
        char *colon = strchr(users[i].passcode, ':');
        const char *display = colon ? colon + 1 : users[i].passcode;
        printf("%d: Name=\"%s\", Scrambled Passcode=\"%s\"\n",
               i + 1, users[i].name, display);
    }
}

/* ── initialize_users ────────────────────────────────────────────────────── */
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

    for (int i = 0; i < n; i++) {
        save_user(&users[i]);
    }

    print_users(n, users);
    free_users(users);
}

/* ── free_users ──────────────────────────────────────────────────────────── */
void free_users(User *users) {
    free(users);
}

/* ── login_user ──────────────────────────────────────────────────────────── *
 * Looks up the stored "SALT:SCRAMBLED" for the given name, extracts the salt,
 * re-scrambles the input password with that salt, then compares.
 * ─────────────────────────────────────────────────────────────────────────── */
int login_user(User *out_user) {
    char name[50];
    char raw[52];

    printf("Enter name: ");
    if (!fgets(name, sizeof(name), stdin)) return 0;
    char *p = strchr(name, '\n');
    if (p) *p = '\0';

    printf("Enter password: ");
    if (!fgets(raw, sizeof(raw), stdin)) return 0;
    p = strchr(raw, '\n');
    if (p) *p = '\0';

    /* Fetch the stored "SALT:SCRAMBLED" from the database */
    User stored;
    if (!get_stored_passcode(name, &stored)) {
        printf("Login failed. Invalid name or password.\n");
        return 0;
    }

    /* Extract the salt from the stored passcode */
    char *colon = strchr(stored.passcode, ':');
    if (!colon) {
        printf("Login failed. Corrupted passcode format.\n");
        return 0;
    }
    *colon = '\0';
    unsigned int salt = (unsigned int)atoi(stored.passcode);
    const char *stored_scrambled = colon + 1;

    /* Scramble the login input with the same salt */
    char attempt_scrambled[51];
    substitution_cipher(raw, attempt_scrambled, salt);

    if (strcmp(attempt_scrambled, stored_scrambled) == 0) {
        strncpy(out_user->name, stored.name, 50);
        strncpy(out_user->passcode, stored.passcode, 62);
        return 1;
    }

    printf("Login failed. Invalid name or password.\n");
    return 0;
}
