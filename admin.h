#ifndef ADMIN_H
#define ADMIN_H

#define ADMIN_CODE_LENGTH 222

extern unsigned char adminCode[ADMIN_CODE_LENGTH];
extern unsigned char resetCode[ADMIN_CODE_LENGTH];

void generate_admin_codes(void);
void save_admin_codes(void);
int load_admin_codes(void);
void shuffle(unsigned char arr[ADMIN_CODE_LENGTH]);
void printcode(unsigned char arr[ADMIN_CODE_LENGTH]);

#endif
