#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "admin.h"


void shuffle(unsigned char adminCode[222]) {

    for (int i = 221; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = adminCode[i];
        adminCode[i] = adminCode[j];
        adminCode[j] = temp;
    }
}

void printcode(adminCode[222]) {
    for (int i = 0; i < 222; i++) {
        
        printf("%c\n",(char)adminCode[i]);
    }
}

void main() {
    srand(time(NULL));

    int j = 0;
    for (int i = 33; i < 256; i++) {
            adminCode[j] = (unsigned char) i;
            j++;
    }
    int j = 0;
    for (int i = 33; i < 256; i++) {
            resetCode[j] = (unsigned char) i;
            j++;
    }

    shuffle(adminCode);
    printcode(adminCode);
    shuffle(resetCode);
    printcode(resetCode);
}
