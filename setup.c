#include <stdio.h>
#include "admin.h"

int main() {

    printf("Generating admin credentials...\n");

    generate_admin_codes();
    save_admin_codes();

    printf("Admin credentials saved to admin.bin\n");

    return 0;
}
