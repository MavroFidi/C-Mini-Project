#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "admin.h"

unsigned char adminCode[222];
unsigned char resetCode[222];


void shuffle(unsigned char adminCode[222]) {

    for (int i = 221; i > 0; i--) {
        int j = rand() % (i + 1);

        unsigned char temp = adminCode[i];
        adminCode[i] = adminCode[j];
        adminCode[j] = temp;
    }
}

void printcode(unsigned char adminCode[222]) {
    for (int i = 0; i < 222; i++) {
        
        printf("%c",(char)adminCode[i]);
    }
}

void save_admin_codes()
{
    FILE *f = fopen("admin.bin", "wb");
    if (!f) {
        printf("Failed to save admin codes\n");
        exit(1);
    }

    fwrite(adminCode, sizeof(unsigned char), 222, f);
    fwrite(resetCode, sizeof(unsigned char), 222, f);

    fclose(f);
}

int load_admin_codes() {
    FILE *f = fopen("admin.bin", "rb");
    if (!f) {
        printf("Admin file missing!\n");
        return 0;
    }

    fread(adminCode, sizeof(unsigned char), 222, f);
    fread(resetCode, sizeof(unsigned char), 222, f);

    fclose(f);
    return 1;
}

void generate_admin_codes() {
    srand(time(NULL));

    int j = 0;
    for (int i = 33; i < 256; i++) {
            adminCode[j] = (unsigned char) i;
            j++;
    }
    j=0;
    for (int i = 33; i < 256; i++) {
            resetCode[j] = (unsigned char) i;
            j++;
    }

    shuffle(adminCode);
    shuffle(resetCode); 
}
