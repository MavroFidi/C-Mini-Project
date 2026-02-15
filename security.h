#ifndef SECURITY_H
#define SECURITY_H

#define CODE_LENGTH 50

unsigned char (*initializeCodes(int numArrays))[CODE_LENGTH];
void cleanupCodes(void *ptr);

#endif
