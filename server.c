#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "keyValueStore.h"

#define GROESSE 1024
#define SCHLEIFEFUERTELNET 1
#define PORT 5678

int server_fd; // Filedeskriptor/Rendevouzdescriptor des Sockets
int user; // Verbindungsdeskriptor des Users / Client
pid_t childpid;
struct sockaddr_in client; // Socketadresse eines Clients

int startServer() {
    char* key;
    char* value;
    char* order;
    char sendingMessage[GROESSE];

    socklen_t client_len; // Länge der Client-Daten
    char incomingMessage[GROESSE]; // Daten vom Client an den Server
    int messageSize; // Anzahl der Bytes, die der Client geschickt hat


    // Erstellen / Anlegen von mySocket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCKSTREAM = TCP (verbindungsorientiert), 0 =
    if ( server_fd < 0 ) {
        fprintf(stderr, "mySocket konnte nicht erstellt werden\n");
        exit(-1);
    }

    //Optionales binden für schnellere Verbindung
    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));
    //Server erstellen
    struct sockaddr_in server; // Socketadresse
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // binden an jede meiner IP-Adressen
    server.sin_port = htons(PORT); // Port des Prozesses

    // Serveradresse wird an meinen Socket gebunden
    int serverBindung = bind(server_fd, (struct sockaddr *) &server, sizeof(server)); // mySocket = File-descriptor, server = Adresse im Netzwerk an die das Socket gebunden wird, sizeof(server) = Größe des zweiten Parameters
    if (serverBindung < 0 ) {
        fprintf(stderr, "mySocket konnte nicht gebunden werden\n");
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stderr,"Socket wurde gebunden\n");
    }

    //Listener funktionen
    int listener = listen(server_fd, 5); // mySocket = Socketserver, 5 = Anzahl der Clients
    if (listener < 0 ) {
        fprintf(stderr, "mySocket konnte nicht listen gesetzt werden\n");
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stderr,"Listen wurde erfolgreich aktiviert\n");
    }

    createSharedMemoryStore();
    client_len = sizeof( (struct sockaddr *) &server);

    while (SCHLEIFEFUERTELNET) {

        user = accept(server_fd, (struct sockaddr *) &client, &client_len);

        childpid = fork();

        if (childpid == -1) {
            fprintf(stderr, "Fehler beim forken.\n");
        } else if (childpid == 0) {
            close(server_fd);
            messageSize = read(user, incomingMessage, GROESSE);

            while(messageSize > 2 ) {
                fprintf(stderr, "Die Nachricht enthielt: %d Bytes\n", messageSize);
                memset(sendingMessage,0,GROESSE);
                order = strtok(incomingMessage, " \r");
                key = strtok(NULL," \r");
                value = strtok(NULL," ");

                write(user, sendingMessage, GROESSE);

                if (strcmp(order, "QUIT") == 0) {
                    printf("Connection closed\n");
                    shutdown(user, 2);
                    close(user);
                    return 0;
                } else if (strcmp(order, "PUT") == 0) {
                    int resultPut = put(key, value);
                    if (resultPut == 0 || resultPut == 1) {
                        strcat(sendingMessage, "> ");
                        strcat(sendingMessage, order);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, key);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, value);
                        write(user, sendingMessage, GROESSE);
                    }
                } else if (strcmp(order, "GET") == 0) {
                    if (get(key, value) == 0) {
                        strcat(sendingMessage, "> ");
                        strcat(sendingMessage, order);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, key);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, value);
                        write(user, sendingMessage, GROESSE);
                    }
                    else {
                        strcat(sendingMessage, "> ");
                        strcat(sendingMessage, order);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, key);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, "key_nonexistent\n");
                        write(user, sendingMessage, GROESSE);
                    }

                } else if (strcmp(order, "DEL") == 0){
                    if (del(key) == 0 ) {
                        strcat(sendingMessage, "> ");
                        strcat(sendingMessage, order);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, key);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, "key_deleted\n");
                        write(user, sendingMessage, GROESSE);
                    } else {
                        strcat(sendingMessage, "> ");
                        strcat(sendingMessage, order);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, key);
                        strcat(sendingMessage, ":");
                        strcat(sendingMessage, "key_nonexistent\n");
                        write(user, sendingMessage, GROESSE);
                    }
                }

                //write(user, incomingMessage, messageSize); //Nutzen um ein Echo zurückzusenden
                messageSize = read(user, incomingMessage, GROESSE);
            }
        }

    }
    close(server_fd);
    return 0;
}