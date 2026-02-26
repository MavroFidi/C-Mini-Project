#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "admin.h"

unsigned char adminCode[1];
unsigned char resetCode[1];

void shuffle(unsigned char arr[ADMIN_CODE_LENGTH]) {
    for (int i = ADMIN_CODE_LENGTH - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void printcode(unsigned char arr[ADMIN_CODE_LENGTH]) {
    for (int i = 0; i < ADMIN_CODE_LENGTH; i++) {
        printf("%c", (char)arr[i]);
    }
}

void save_admin_codes(void) {
    FILE *f = fopen("admin.bin", "wb");
    if (!f) {
        printf("Failed to save admin codes\n");
        exit(1);
    }
    fwrite(adminCode, sizeof(unsigned char), ADMIN_CODE_LENGTH, f);
    fwrite(resetCode, sizeof(unsigned char), ADMIN_CODE_LENGTH, f);
    fclose(f);
}

int load_admin_codes(void) {
    FILE *f = fopen("admin.bin", "rb");
    if (!f) {
        printf("Admin file missing!\n");
        return 0;
    }
    fread(adminCode, sizeof(unsigned char), ADMIN_CODE_LENGTH, f);
    fread(resetCode, sizeof(unsigned char), ADMIN_CODE_LENGTH, f);
    fclose(f);
    return 1;
}

void generate_admin_codes(void) {
    srand((unsigned int)time(NULL));

    int j = 0;
    for (int i = 33; i < 256; i++) {
        adminCode[j] = (unsigned char)i;
        resetCode[j] = (unsigned char)i;
        j++;
    }

    shuffle(adminCode);
    shuffle(resetCode);
}
