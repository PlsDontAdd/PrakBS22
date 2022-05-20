#ifndef BS_PRAKTIKUM_SS_22_KEYVALSTORE_H
#define BS_PRAKTIKUM_SS_22_KEYVALSTORE_H

void createSharedMemoryStore();
int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif //BS_PRAKTIKUM_SS_22_KEYVALSTORE_H