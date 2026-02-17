#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "admin.h"

unsigned char adminCode[256];
unsigned char resetCode[16];

void shuffle(unsigned char adminCode[256]) {

    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = adminCode[i];
        adminCode[i] = adminCode[j];
        adminCode[j] = temp;
    }
}

void resetCodeInitial(unsigned char adminCode[256]) {
    for (int i = 0; i < 256; i+=16) {  
            resetCode[i/16] = adminCode[i];
    }

    for (int i = 15; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = resetCode[i];
        resetCode[i] = resetCode[j];
        resetCode[j] = temp;
    }

}

void printcode(adminCode[256]) {
    for (int i = 0; i < 256; i++) {
        
        printf("%c\n",(char)adminCode[i]);
    }
}

void main() {
    srand(time(NULL));

    for (int i = 0; i < 256; i++) {
        adminCode[i] = (unsigned char)i;
    }

    shuffle(adminCode);
    printcode(adminCode);
    resetCodeInitial(adminCode);
}
