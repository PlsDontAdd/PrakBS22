#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <pwd.h>
#include "keyValueStore.h"
#include <sys/shm.h>

#define STORESIZE 500
#define LENGTH_KEY 1024
#define LENGTH_VALUE 1024
#define SHAREDMEMSIZE (((LENGTH_KEY + LENGTH_VALUE) * STORESIZE) + sizeof(int))

typedef struct node {
    char key[LENGTH_KEY];
    char value[LENGTH_VALUE];
    // struct node *next;
}node;

int shmid;
int* keyValueNum;
struct node* keyValueStore;


void createSharedMemoryStore(void) {
    printf("Shared Memory Store: Starting Initialization!\n");
    char *shm_addr;

    // Create Shared Memory
    shmid = shmget(IPC_PRIVATE, SHAREDMEMSIZE, IPC_CREAT | SHM_R | SHM_W);
    if (shmid == -1) {
        printf("Shared Memory Store: Error at creating Shared Memory: Key: %d | Größe: %ld\n", IPC_PRIVATE, SHAREDMEMSIZE);
    } else {
        printf("Shared Memory Store: Shared Memory successfully created! SharedMemoryID: %d\n", shmid);
    }

    // Bind Shared Memory
    printf("Shared Memory Store: Binding Shared Memory!\n");
    shm_addr = shmat(shmid, NULL, 0);
    if (!shm_addr) {
        printf("Shared Memory Store: Error at binding Shared Memory!\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Shared Memory Store: Successfully binded Shared Memory!\n");
    }

    // Shared Memory keyValueNum = Number of Keys in struct
    printf("Shared Memory Store: Creating keyValueStore in Shared Memory!\n");
    keyValueNum = (int*) shm_addr;
    *keyValueNum = 0;

    // Shared Memory keyValueStore = Range of keyValueStore
    keyValueStore = (struct node*) ((void*) shm_addr + sizeof(int));
    //printf("Shared Memory Store: KeyValueStore: %s\n", *keyValueStore);
    if (keyValueStore == (void*) -1) {
        printf("Shared Memory Store: Error at creating KeyValueStore!\n");
    } else {
        printf("Shared Memory Store: Key-Value-Store created!\n");
    }
    printf("Shared Memory Store: Key-Value-Number %d\n", *keyValueNum);
}

void deleteSharedMemoryStore() {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("Shared Memory Store: Error at deleting Shared Memory Store!\n");
        printf("Shared Memory Store: SharedMemoryID: %d, Command: %d\n", shmid, IPC_RMID);
    } else {
        printf("Shared Memory Store: Shared Memory deleted!\n");
    }
}

int put(char* key, char* value) {
    int i;

    for (i = 0; i < (*keyValueNum); i++) {
        if (strcmp(keyValueStore[i].key, key) == 0) {
            strcpy(keyValueStore[i].value, value);
            return 0;
        }
    }

    if (((*keyValueNum) + 1) < STORESIZE) {
        strcpy(keyValueStore[(*keyValueNum)].key, key);
        strcpy(keyValueStore[(*keyValueNum)].value, value);
        (*keyValueNum)++;
    }
    return 0;
}

int get(char* key, char* res) {
    int i = 0;
    if(strcmp(keyValueStore[i].key, "\0") != 0) {
        do {
            if(strcmp(keyValueStore[i].key, key) == 0) {
                strcpy(res, keyValueStore[i].value);
                return 0;
            }
            i++;
        } while ((strcmp(keyValueStore[i].key, "\0") != 0));
        return -2;
    } else {
        return -2;
    }
}

int del(char* key) {
    int i = 0;
    if(strcmp(keyValueStore[i].key, "\0") != 0) {
        do {
            if(strcmp(keyValueStore[i].key, key) == 0) {
                int j = i + 1;

                do {
                    strcpy(keyValueStore[i].key, keyValueStore[j].key);
                    strcpy(keyValueStore[i].value, keyValueStore[j].value);
                    j++;
                    i++;
                } while ((strcmp(keyValueStore[j - 1].key, "\0") != 0));
                (*keyValueNum)--;

                return 0;
            }
            i++;
        } while ((strcmp(keyValueStore[i].key, "\0") != 0));
        return -1;
    }
    else {
        return -1;
    }
}