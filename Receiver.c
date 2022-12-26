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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock <= 0)
    {
        perror("socket creation failed");
        close(sock);
        exit(1);
    }

    int enableReuse = 1; 
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("socket setting failed");
        close(sock);
        exit(1);
    }
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); 
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < -1)
    {
        perror("binding failed");
        close(sock);
        exit(1);
    }

    if(listen(sock, 1) < 0)
    {
        perror("listening failed");
        close(sock);
        exit(1);
    }

    printf("Waiting for incoming TCP-connection...\n");

    struct sockaddr_in clientAddress; 
    socklen_t clientAddressLen = sizeof(clientAddress); 

    memset(&clientAddress, 0, sizeof(clientAddress)); 
    clientAddressLen = sizeof(clientAddress); 

    int sock2 = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if(sock2 <= 0)
    {
        return -1;
    }

    printf("A new client connection accepted\n");

    char buffer[BUFSIZ];
    memset(buffer, '\0', BUFSIZ);

    if(recv(sock2, buffer, BUFSIZ, 0) <= 0)
    {
        printf("err");
        return 0;
    }
    printf("message: %s", buffer);

    return 0;
}