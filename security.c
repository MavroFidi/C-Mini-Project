#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "security.h"
#include "admin.h"

/* ── initialize_security ─────────────────────────────────────────────────── */
void initialize_security(void) {
    /* Seed RNG once for the session. */
    srand((unsigned int)time(NULL));
    printf("Security engine initialized.\n");
}

/* ── verify_admin ─────────────────────────────────────────────────────────── */
int verify_admin(unsigned char *input) {
    /* Strip trailing newline that fgets may have left. */
    size_t len = strlen((char *)input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
        len--;
    }

    if (len != ADMIN_CODE_LENGTH) return 0;
    return (memcmp(input, adminCode, ADMIN_CODE_LENGTH) == 0) ? 1 : 0;
}

/* ── admin_panel ──────────────────────────────────────────────────────────── */
void admin_panel(void) {
    printf("\n==== ADMIN PANEL ====\n");

    int choice;
    int running = 1;

    while (running) {
        printf("\n1. Initialize user codes\n");
        printf("2. Print admin code\n");
        printf("3. Exit admin panel\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: {
                int n;
                printf("How many user codes? ");
                if (scanf("%d", &n) != 1 || n <= 0) {
                    while (getchar() != '\n');
                    printf("Invalid number.\n");
                    break;
                }
                while (getchar() != '\n');

                unsigned char (*codes)[CODE_LENGTH] = initializeCodes(n);
                if (codes) {
                    printUserCodes(n, codes);
                    cleanupCodes(codes);
                }
                break;
            }
            case 2:
                printf("Admin code: ");
                printcode(adminCode);
                printf("\n");
                break;
            case 3:
                running = 0;
                break;
            default:
                printf("Invalid option.\n");
        }
    }
}

/* ── initializeCodes ─────────────────────────────────────────────────────── */
unsigned char (*initializeCodes(int numArrays))[CODE_LENGTH] {
    unsigned char (*userCodes)[CODE_LENGTH] = malloc(numArrays * sizeof(*userCodes));
    if (!userCodes) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < numArrays; i++) {
        char buf[128];

        while (1) {
            printf("Enter user code %d (50 chars): ", i + 1);
            if (!fgets(buf, sizeof(buf), stdin)) {
                buf[0] = '\0';
            }

            /* Remove trailing newline */
            size_t len = strlen(buf);
            if (len > 0 && buf[len - 1] == '\n') {
                buf[--len] = '\0';
            } else {
                /* Flush leftover input */
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
            }

            if (len == CODE_LENGTH) {
                memcpy(userCodes[i], buf, CODE_LENGTH);
                break;
            }
            printf("Invalid length (%zu). Please enter exactly 50 characters.\n", len);
        }
    }

    return userCodes;
}

/* ── printUserCodes ──────────────────────────────────────────────────────── */
void printUserCodes(int count, unsigned char (*userCodes)[CODE_LENGTH]) {
    for (int i = 0; i < count; i++) {
        printf("User Code %d: ", i + 1);
        for (int j = 0; j < CODE_LENGTH; j++) {
            printf("%c", userCodes[i][j]);
        }
        printf("\n");
    }
}

/* ── cleanupCodes ────────────────────────────────────────────────────────── */
void cleanupCodes(void *ptr) {
    free(ptr);
}
