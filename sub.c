#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include "keyValueStore.h"
#include "server.h"

typedef struct sublist {
    char key[LENGTH_KEY];
    int pids[10];
} sublist;

struct text_message {
    long mtype;
    char mtext[GROESSE];
};

struct sublist* subs;

int shmid, semid, msgid;

void initSub() {
    char *shm_addr;

    // Create Shared Memory
    shmid = shmget(IPC_PRIVATE, sizeof(struct sublist)* STORESIZE, IPC_CREAT | SHM_R | SHM_W);
    if (shmid == -1) {
        printf("Shared Memory Store: Error at creating Shared Memory\n");
    } else {
        printf("Shared Memory Store: Shared Memory successfully created! SharedMemoryID: %d\n", shmid);
    }

    shm_addr = shmat(shmid, NULL, 0);
    subs = (struct sublist*) shm_addr;

    //semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0777);
    msgid = msgget(1, IPC_CREAT|0644);

//    char cleanString[1024];
//
//    clearArray(cleanString);
//
//    for (int i = 0; i < STORESIZE; ++i) {
//        strcpy(subs[i].key, cleanString);
//        for (int j = 0; j < 10; ++j) {
//            subs[i].pids[j] = 0;
//        }
//    }
}

int addKey(char* key) {
    for (int i = 0; i < STORESIZE; ++i) {
        if (strcmp(subs[i].key, "\0") == 0) {
            strcpy(subs[i].key, key);
            return i;
        }
    }
    return -1;
}

int getKeyNum(char* key) {
    for (int i = 0; i < STORESIZE; ++i) {
        if (strcmp(subs[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void saveSub(char* key, int id) {
    int num = getKeyNum(key);

    if (num == -1) {
        num = addKey(key);
    }

    for (int i = 0; i < 10; ++i) {
        if (subs[num].pids[i] == 0) {
            subs[num].pids[i] = id;
            return;
        }
    }
}

void sendMessage(char* key, char* message, int pid) {
    int num;
    struct text_message msg;

    num = getKeyNum(key);

    if (num != -1) {
        strcpy(msg.mtext, message);
        for (int i = 0; i < 10; ++i) {
            if (subs[num].pids[i] != 0 && subs[num].pids[i] != pid) {
                printf("Send message for: %i", subs[num].pids[i]);
                msg.mtype = subs[num].pids[i];
                msgsnd(msgid, &msg, 1024, 0);
            }
        }
    }
}

void receiveMessage(int client, int pid) {
    char message[GROESSE];
    struct text_message msg;

    memset(message, 0, GROESSE);
    while (1) {
        clearArray(message);

        msgrcv(msgid, &msg, 1024, pid, 0);
        strcpy(message, msg.mtext);

        write(client, message, 1024);
    }
}