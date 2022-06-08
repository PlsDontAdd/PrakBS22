#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H

void initSub();
void saveSub(char* key, int id);
void sendMessage(char* key, char* message, int pid);
void receiveMessage(int client, int pid);

#endif //PRAKBS22_SUB_H
