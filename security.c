#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "security.h"

unsigned char accessCode[256];
unsigned char resetCode[256];
unsigned char arr[256];

void shuffle(unsigned char arr[256]) {

    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void codeInitial() {

    srand(time(NULL));
    for (int i = 0; i < 256; i++) {
        arr[i] = i;
    }
    shuffle(arr);
}

int main() {
    codeInitial();
    for (int i = 0; i < 256; i++) {
        accessCode[i] = arr[i];
    }
}
