#include <stdio.h>
#include "admin.h"

void main(void) {
    printf("Generating admin credentials...\n");

    generate_admin_codes();
    save_admin_codes();

    printf("Admin code:  ");

    printf("\n");
    
    printcode(adminCode);
    
    printf("\n");
    
    printf("\nAdmin credentials saved to admin.bin\n");
    printf("Keep admin.bin in the same folder as app.exe\n");
}
