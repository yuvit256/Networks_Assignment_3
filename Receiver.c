// Authors: Yuval Musseri & Maor Berenstein:

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

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0); // creating the listening socket to establish a connection
    if (listeningSocket <= 0)                              // Checking if the socket opened proparlly
    {
        perror("socket creation failed");
        close(listeningSocket);
        exit(1);
    }

    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0) // 
    {
        perror("socket setting failed");
        close(listeningSocket);
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress)); // reset
    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(PORT);              // translates an integer from host byte order to network byte order
    serverAddress.sin_addr.s_addr = INADDR_ANY;        // recives any IP as an address which can communicate with it.

    if (bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < -1) // we bind the socket with the settings we set above.
    {
        perror("binding failed");
        close(listeningSocket);
        exit(1);
    }

    if (listen(listeningSocket, 1) < 0) // here we "listen" and wait for a client to connect and by that start the communication.
    {
        perror("listening failed");
        close(listeningSocket);
        exit(1);
    }

    printf("Waiting for incoming TCP-connection...\n"); // we wait for the client to send a connection request.

    struct sockaddr_in clientAddress; // we set our preferences for the client socket
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress)); // reset
    clientAddressLen = sizeof(clientAddress);

    int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen); // Await a connection on socket, When a connection arrives, open a new socket to communicate with it
    if (clientSocket <= 0)                                                                            // checks if the connection has done proparlly
    {
        perror("client socket acception failed.");
        close(listeningSocket);
        exit(1);
    }

    printf("A new client connection accepted\n");

    char a = '0'; // answer from the user if to continue
    char *answer = &a;

    double sumFirstHalfTime = 0, sumSecondHalfTime= 0; // here we define the variable who hold all the running time for all the files.
    int counter = 0; // here we define the counter which counts the times the sender sent the file.
Continue:

    while (true)
    {
        char buffer[BUFSIZ] = {'\0'};
        long bytes = 0;

        gettimeofday(&before, NULL);

        while (bytes != 678259) // receive first part
        {
            bytes += recv(clientSocket, buffer, BUFSIZ, 0);
        }

        gettimeofday(&after, NULL);

        timersub(&after, &before, &difference);

        double realTime1 = ((difference.tv_sec) * 1000000.0 + difference.tv_usec) / 1000.0; // calculating the time it took in miliseconds

        printf("times: %f\n", realTime1); // output the time

        char *authKey = YxM; // in this String var we store the AUTH Key
        if (send(clientSocket, authKey, strlen(authKey) + 1, 0) == -1) // check if the send function works properly if it does, send the auth key which contains 
                                                                        // Maor's xor Yuval's IDs in binary to the client.
        {
            printf("Error in sending the N for exit command\n");// if send returns -1 print error and close communication.
            close(clientSocket);
            close(listeningSocket);
            exit(1);
        }; // send authentication

        char *algo = "reno";
        if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, &algo, sizeof(algo))<0){
            printf("Error in changinc CC algorithem to reno\n");
            close(clientSocket);
            close(listeningSocket);
            exit(1);
        }; // change CC
        printf("congestion changed!\n");

        bzero(buffer, BUFSIZ);
        bytes = 0;
        gettimeofday(&before, NULL);
        while (bytes != 678260) // receive second part
        {
            bytes += recv(clientSocket, buffer, BUFSIZ, 0);
        }


        *algo = "cubic";
        if(setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, &algo, sizeof(algo))<0){
            printf("Error in changinc CC algorithem to reno\n");
            close(clientSocket);
            close(listeningSocket);
            exit(1);
        }

        gettimeofday(&after, NULL);

        timersub(&after, &before, &difference);

        double realTime2 = ((difference.tv_sec) * 1000000.0 + difference.tv_usec) / 1000.0;
        printf("times: %f\n", realTime2); // output the time

        recv(clientSocket, answer, 3, 0);
        printf("Answer = %c \n", a);

        // Calculate the time sum and then we do the avg

        counter++;
        sumFirstHalfTime += realTime1;
        sumSecondHalfTime += realTime2;

        if (a == 'Y')
        {
            goto Continue;
        }
        else if (a == 'N')
        {
            printf("Time it took to send the first half:%lf \n", realTime1);
            printf("Time it took to send the second half:%lf \n", realTime2);
            printf("The avg time it took to send the first half:%lf \n", sumFirstHalfTime / counter);
            printf("The avg time it took to send the second half:%lf \n", sumSecondHalfTime / counter);

            break;
        }
    }

    close(listeningSocket);
    close(clientSocket);
    exit(0);
}