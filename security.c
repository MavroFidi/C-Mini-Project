#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "security.h"
#include "admin.h"

unsigned char (*initializeCodes(int numArrays)) [CODE_LENGTH] {
    unsigned char (*userCodes)[CODE_LENGTH] = malloc(numArrays * sizeof(*userCodes));
    if (userCodes == NULL) {
        return NULL;
    }

    srand(time(NULL));
    for (int i = 0; i < numArrays; i++) {
    char Usercode[100];

    while (1) {
        printf("Enter the user code %i: ", i);
        fgets(Usercode, sizeof(Usercode), stdin);

        size_t len = strlen(Usercode);
        if (len > 0 && Usercode[len - 1] == '\n') {
            Usercode[len - 1] = '\0';
        }

        if (len == sizeof(Usercode) - 1 && Usercode[len - 1] != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        }

        if (len == 50) {
            break;
        }
        else {
            printf("Invalid user code. Please enter a 50-character code.\n");
        }
    }
}

    return userCodes;
}

void shuffle(unsigned char arr[CODE_LENGTH]) {

    for (int i = CODE_LENGTH - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void printcode(int a) {
    for (int i = 0; i < a; i++) {
        printf("User Code %i: ");
        for (int j = 0;; j < 222; j++) {
            printf("%c", usercode[i][j]);
        }
        printf("\n");
    }
}

void main() {
    char *admincode;
    printf("Enter the admin code: ");
    scanf("%s", admincode);

    if (memcmp(argv[1], adminCode, 222) == 0) {
        int numArrays;
        printf("Enter the number of user codes you wish to initialize. ");
        scanf("%i", &numArrays);
        unsigned char (*userCodes)[CODE_LENGTH] = initializeCodes(numArrays);
        printcode(numArrays);
    }
    else {
        printf("Access Denied\n");
    }
}
