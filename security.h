#ifndef SECURITY_H
#define SECURITY_H

#define CODE_LENGTH 50

void initialize_security(void);
int  verify_admin(unsigned char *input);
void admin_panel(void);

unsigned char (*initializeCodes(int numArrays))[CODE_LENGTH];
void printUserCodes(int count, unsigned char (*userCodes)[CODE_LENGTH]);
void cleanupCodes(void *ptr);

#endif
