#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

#define CLIENT_MESSAGE_SIZE 2048 //Some machines are testing for more than 1024 (TestCase 9)
#define CLIENT_ARRAY_SIZE 2048
#define BUFFER_SIZE 120000

typedef struct Client {
    int     id;
    char    message[CLIENT_MESSAGE_SIZE];
} Client;

Client      clientList[CLIENT_ARRAY_SIZE];
fd_set      readSet, writeSet, activeSet;
int         maxFd = 0, nextClientId = 0;
char        readBuffer[BUFFER_SIZE], writeBuffer[BUFFER_SIZE];

void displayError(char *message) {
    if (message)
        write(2, message, strlen(message));
    else
        write(2, "Fatal error", strlen("Fatal error"));
    write(2, "\n", 1);
    exit(1);
}

void broadcastMessage(int exceptionFd) {
    for(int i = 0; i <= maxFd; i++)
        if(FD_ISSET(i, &writeSet) && i != exceptionFd)
            send(i, writeBuffer, strlen(writeBuffer), 0);
}

void handleNewConnection(int serverFd, struct sockaddr_in *serverAddress) {
    socklen_t len;
    int newClientFd = accept(serverFd, (struct sockaddr *)serverAddress, &len);
    if (newClientFd < 0)
        return;
    maxFd = newClientFd > maxFd ? newClientFd : maxFd;
    clientList[newClientFd].id = nextClientId++;
    FD_SET(newClientFd, &activeSet);
    sprintf(writeBuffer, "server: client %d just arrived\n", clientList[newClientFd].id);
    broadcastMessage(newClientFd);
}

void handleMessage(int clientFd) {
    int bytesRead = recv(clientFd, readBuffer, sizeof(readBuffer), 0);
    if (bytesRead <= 0) {
        sprintf(writeBuffer, "server: client %d just left\n", clientList[clientFd].id);
        broadcastMessage(clientFd);
        FD_CLR(clientFd, &activeSet);
        close(clientFd);
        return;
    }
    for (int i = 0, j = strlen(clientList[clientFd].message); i < bytesRead; i++, j++) {
        clientList[clientFd].message[j] = readBuffer[i];
        if (clientList[clientFd].message[j] == '\n') {
            clientList[clientFd].message[j] = '\0';
            sprintf(writeBuffer, "client %d: %s\n", clientList[clientFd].id, clientList[clientFd].message);
            broadcastMessage(clientFd);
            bzero(&clientList[clientFd].message, strlen(clientList[clientFd].message));
            j = -1;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2)
        displayError("Wrong number of arguments");

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        displayError(NULL);

    FD_ZERO(&activeSet);
    bzero(&clientList, sizeof(clientList));
    maxFd = serverFd;
    FD_SET(serverFd, &activeSet);

    struct sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1
    serverAddress.sin_port = htons(atoi(argv[1]));

    if ((bind(serverFd, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0)
        displayError(NULL);
    if (listen(serverFd, 10) < 0)
        displayError(NULL);

    while(1) {
        readSet = writeSet = activeSet;
        if (select(maxFd + 1, &readSet, &writeSet, NULL, NULL) < 0)
            continue;
        for(int fdIndex = 0; fdIndex <= maxFd; fdIndex++) {
            if (FD_ISSET(fdIndex, &readSet)) {
                if (fdIndex == serverFd) {
                    handleNewConnection(serverFd, &serverAddress);
                } else {
                    handleMessage(fdIndex);
                }
            }
        }
    }
}