#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "security.h"


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

void shuffle(unsigned char arr[50]) {

    for (int i = 49; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


void main(int argc, char *argv[]) {
    int numArrays = atoi(argv[1]);
    unsigned char (*userCodes)[CODE_LENGTH] = initializeCodes(numArrays);
}
