#ifndef BS_PRAKTIKUM_SS_22_KEYVALSTORE_H
#define BS_PRAKTIKUM_SS_22_KEYVALSTORE_H

#define STORESIZE 500
#define LENGTH_KEY 1024
#define LENGTH_VALUE 1024
#define SHAREDMEMSIZE ((sizeof(node) * STORESIZE) + sizeof(int) * 2)

void createSharedMemoryStore();
void deleteSharedMemoryStore();
int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);
int beg();
int end();

#endif //BS_PRAKTIKUM_SS_22_KEYVALSTORE_H