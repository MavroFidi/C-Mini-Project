#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "security.h"
#include "admin.h"


unsigned char (*initializeCodes(int numArrays))[CODE_LENGTH] {
    unsigned char (*userCodes)[CODE_LENGTH] = malloc(numArrays * sizeof(*userCodes));
    if (userCodes == NULL) return NULL;
    
    srand(time(NULL));
    for (int i = 0; i < numArrays; i++) {
         for (int j = 0; j < CODE_LENGTH; j++) {
            userCodes[i][j] = j;
        }
        shuffle(userCodes[i]);
    }
    
    return userCodes;
}

void shuffle(unsigned char arr[CODE_LENGTH]) {

    for (int i = CODE_LENGTH-1; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


void main(int argc, char *argv[]) {
    if (memcmp(argv[1], adminCode, 222) == 0) {
        int numArrays = atoi(argv[2]);
        unsigned char (*userCodes)[CODE_LENGTH] = initializeCodes(numArrays);
    }
    else {
        printf("Access Denied\n");
    }
}
