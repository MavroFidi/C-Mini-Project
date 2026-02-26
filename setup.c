#include <stdio.h>
#include "admin.h"

int main(void) {
    printf("Generating admin credentials...\n");

    adminCode[0] = 'a';
    resetCode[0] = 'a';

    printf("Admin code:  "); printcode(adminCode); printf("\n");
    printf("Reset code:  "); printcode(resetCode); printf("\n");
    printf("\nAdmin credentials saved to admin.bin\n");
    printf("Keep admin.bin in the same folder as app.exe\n");

    return 0;
}
