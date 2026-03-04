#include <stdio.h>
#include "admin.h"

int main(void) {
    printf("Generating admin credentials...\n");

    generate_admin_codes();
    save_admin_codes();

    printf("Admin code:  "); printcode(adminCode); printf("\n");
    printf("\nAdmin credentials saved to admin.bin\n");
    printf("Keep admin.bin in the same folder as app.exe\n");

    return 0;
}
