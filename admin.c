#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "admin.h"

unsigned char adminCode[256];

void shuffle(unsigned char adminCode[256]) {

    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = adminCode[i];
        adminCode[i] = adminCode[j];
        adminCode[j] = temp;
    }
}

void main() {
    srand(time(NULL));

    for (int i = 0; i < 256; i++) {
        adminCode[i] = (unsigned char)i;
    }

    shuffle(adminCode);
}
