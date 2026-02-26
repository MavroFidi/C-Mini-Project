#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char name[50];
    char passcode[16];
} User;

int main(void) {
    int n;
    printf("Enter number of users: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number.\n");
        return 1;
    }
    /* consume leftover newline from scanf */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    User *users = malloc(sizeof(User) * n);
    if (!users) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    for (int i = 0; i < n; i++) {
        printf("Enter name for user %d: ", i + 1);
        if (!fgets(users[i].name, sizeof(users[i].name), stdin)) {
            users[i].name[0] = '\0';
        } else {
            /* remove trailing newline */
            char *p = strchr(users[i].name, '\n');
            if (p) *p = '\0';
        }
        /* simple 6-digit numeric passcode */
        int code = rand() % 900000 + 100000; /* 100000..999999 */
        sprintf(users[i].passcode, "%06d", code);
    }

    printf("\nAssigned users and passcodes:\n");
    for (int i = 0; i < n; i++) {
        printf("%d: Name=\"%s\", Passcode=\"%s\"\n",
               i + 1, users[i].name, users[i].passcode);
    }

    free(users);
    return 0;
}
