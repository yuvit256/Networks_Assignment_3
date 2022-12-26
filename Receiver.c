//SERVER

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

#define PORT 9696
#define IP "127.0.0.1"

int main()
{
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listeningSocket <= 0)
    {
        perror("socket creation failed");
        close(listeningSocket);
        exit(1);
    }

    int enableReuse = 1; 
    if(setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
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
    
    if(bind(listeningSocket, (struct sockaddr *)&server_address, sizeof(server_address)) < -1)
    {
        perror("binding failed");
        close(listeningSocket);
        exit(1);
    }

    if(listen(listeningSocket, 1) < 0)
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
    if(clientSocket <= 0)
    {
        perror("client socket acception failed.");
        close(listeningSocket);
        exit(1);
    }

    printf("A new client connection accepted\n");


    while (1) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
            printf("listen failed with error code : %d", errno);
            // close the sockets
            close(listeningSocket);
            return -1;
        }

        printf("A new client connection accepted\n");

        // Receive a message from client
        char buffer[BUFSIZ];
        memset(buffer, 0, BUFSIZ);
        int bytesReceived = recv(clientSocket, buffer, BUFSIZ, 0);
        if (bytesReceived == -1) {
            printf("recv failed with error code : %d", errno);
            // close the sockets
            close(listeningSocket);
            close(clientSocket);
            return -1;
        }
        
        //receiving the file from the Sender/Client.
        int receivedFile;
        recv(clientSocket,&receivedFile, sizeof(receivedFile), 0);
        if(receivedFile==0){
            printf("The proccess of sending the file ended. \n");
            break;
        }

        
        printf("Received: %s", buffer);

        // Reply to client
        char *message = "Welcome to our TCP-server\n";
        int messageLen = strlen(message) + 1;

        int bytesSent = send(clientSocket, message, messageLen, 0);
        if (bytesSent == -1) {
            printf("send() failed with error code : %d", errno);
            close(listeningSocket);
            close(clientSocket);
            return -1;
        } else if (bytesSent == 0) {
            printf("peer has closed the TCP connection prior to send().\n");
        } else if (bytesSent < messageLen) {
            printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
        } else {
            printf("message was successfully sent.\n");
        }
    }

    close(listeningSocket);

    return 0;





    // char buffer[BUFSIZ];
    // memset(buffer, '\0', BUFSIZ);

    // if(recv(sock2, buffer, BUFSIZ, 0) <= 0)
    // {
    //     printf("err");
    //     return 0;
    // }
    // printf("message: %s", buffer);

    // return 0;
}