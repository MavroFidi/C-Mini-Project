#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "admin.h"
#include "database.h"
#include "users.h"

int main(void) 
{
    srand((unsigned int)time(NULL));
    printf("=== Secure Authentication System ===\n");

    if (!load_admin_codes()) 
    {
        printf("Admin system not initialized.\n");
        printf("Run setup.exe first to generate admin.bin\n");
        return 1;
    }
    printf("Admin codes loaded successfully.\n");

    if (!init_database()) 
    {
        printf("Database failed to initialize.\n");
        return 1;
    }
    printf("Database ready.\n");

    int choice;

    while (1) 
    {
        printf("\n==== MENU ====\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) 
        {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        if (choice == 1) 
        {
            unsigned char input[ADMIN_CODE_LENGTH + 2];

            printf("Enter Admin Code:\n");
            if (!fgets((char *)input, sizeof(input), stdin)) continue;

            size_t len = strlen((char *)input);
            if (len > 0 && input[len - 1] == '\n') input[--len] = '\0';

            if (len == ADMIN_CODE_LENGTH &&
                memcmp(input, adminCode, ADMIN_CODE_LENGTH) == 0) {

                printf("Admin Access Granted.\n");

                int admin_running = 1;
                while (admin_running) 
                {
                    printf("\n==== ADMIN PANEL ====\n");
                    printf("1. Create users\n");
                    printf("2. Print admin code\n");
                    printf("3. Reset Users\n");
                    printf("4. Exit admin panel\n");
                    printf("Choose: ");

                    int admin_choice;
                    if (scanf("%d", &admin_choice) != 1) 
                    {
                        while (getchar() != '\n');
                        continue;
                    }
                    while (getchar() != '\n');

                    switch (admin_choice) 
                    {
                        case 1:
                            initialize_users();
                            break;
                        case 2:
                            printf("Admin code: ");
                            printcode(adminCode);
                            printf("\n");
                            break;
                        case 3:
                            reset_users();
                            printf("Users reset.\n");
                            break;
                        case 4:
                            admin_running = 0;
                            break;
                        default:
                            printf("Invalid option.\n");
                    }
                }

            } 
            else 
            {
                printf("Invalid Admin Code.\n");
            }
        } 
        else if (choice == 2) 
        {
            User logged_in;
            if (login_user(&logged_in)) 
            {
                printf("Welcome, %s!\n", logged_in.name);
                notes_menu(logged_in.name);
            } else 
            {
                printf("Login failed. Invalid name or passcode.\n");
            }

        } else if (choice == 3) 
        {
            printf("Exiting system.\n");
            break;
        } 
        else 
        {
            printf("Invalid option.\n");
        }
    }

    close_database();
    return 0;
}
