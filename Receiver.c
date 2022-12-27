#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

#define PORT 9696
#define IP "127.0.0.1"
#define FILE_SIZE 1356519
#define YxM "0000100000101010"

int main()
{

    struct timeval before = {0}, after = {0}, difference = {0};

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket <= 0)
    {
        perror("socket creation failed");
        close(listeningSocket);
        exit(1);
    }

    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("socket setting failed");
        close(listeningSocket);
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(listeningSocket, (struct sockaddr *)&server_address, sizeof(server_address)) < -1)
    {
        perror("binding failed");
        close(listeningSocket);
        exit(1);
    }

    if (listen(listeningSocket, 1) < 0)
    {
        perror("listening failed");
        close(listeningSocket);
        exit(1);
    }

    printf("Waiting for incoming TCP-connection...\n");

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (clientSocket <= 0)
    {
        perror("client socket acception failed.");
        close(listeningSocket);
        exit(1);
    }

    printf("A new client connection accepted\n");

    while(true)
    {
        char buffer[BUFSIZ] = {'\0'};
        long bytes = 0;

        gettimeofday(&before, NULL);
        while (bytes != 7) // receive first part
        {
            bytes += recv(clientSocket, buffer, BUFSIZ, 0);
        }
        gettimeofday(&after, NULL);

        timersub(&after, &before, &difference);

        double realTime = ((difference.tv_sec) * 1000000.0 + difference.tv_usec) / 1000.0;

        printf("message: %s\ntimes: %f\n", buffer, realTime); // output first part - REMOVE WHEN SENDING BIG FILES


        char *sendSender = YxM;
        send(clientSocket, sendSender, strlen(sendSender) + 1, 0); // send authentication

        char *algo = "reno";
        setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, &algo, sizeof(algo)); // change CC
        printf("congestion changed!\n");

        bzero(buffer, BUFSIZ);
        bytes = 0;
        gettimeofday(&before, NULL);
        while (bytes != 9) // receive second part
        {
            bytes += recv(clientSocket, buffer, BUFSIZ, 0);
        }
        gettimeofday(&after, NULL);

        realTime = ((difference.tv_sec) * 1000000.0 + difference.tv_usec) / 1000.0;

        printf("message: %s\n", buffer);

        printf("closing connection\n");
        
        break;
    }

    close(listeningSocket);
    close(clientSocket);
    exit(0);
}