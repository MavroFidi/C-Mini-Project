#include <stdio.h>
#include <stdlib.h>
#include "admin.h"
#include "security.h"
#include "database.h"

int main(void) {
    printf("=== Secure Authentication System ===\n");

    /* STEP 1 — Load Admin Codes */
    if (!load_admin_codes()) {
        printf("Admin system not initialized.\n");
        printf("Run setup.exe first to generate admin.bin\n");
        return 1;
    }
    printf("Admin codes loaded successfully.\n");

    /* STEP 2 — Initialize database */
    if (!init_database()) {
        printf("Database failed to initialize.\n");
        return 1;
    }
    printf("Database ready.\n");

    /* STEP 3 — Start security engine */
    initialize_security();
    printf("Security engine ready.\n");

    int choice;

    while (1) {
        printf("\n==== MENU ====\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        if (choice == 1) {
            unsigned char input[ADMIN_CODE_LENGTH + 2]; /* +2 for newline + null */

            printf("Enter Admin Code:\n");
            if (!fgets((char *)input, sizeof(input), stdin)) continue;

            if (verify_admin(input)) {
                printf("Admin Access Granted.\n");
                admin_panel();
            } else {
                printf("Invalid Admin Code.\n");
            }
        } else if (choice == 2) {
            user_login();
        } else if (choice == 3) {
            printf("Exiting system.\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    close_database();
    return 0;
}
